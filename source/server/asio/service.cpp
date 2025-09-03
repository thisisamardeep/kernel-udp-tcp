#include "asio/service.h"

#include "errors/fatal.h"

namespace AsioWrapper {
    namespace Asio {

        Service::Service(int threads, bool pool) :
            _strand_required(false), _polling(false), _started(false), _round_robin_index(0) {
            if (threads == 0) {
                _services.emplace_back(std::make_shared<asio::io_service>());
            } else if (!pool) {
                for (int thread = 0; thread < threads; ++thread) {
                    _services.emplace_back(std::make_shared<asio::io_service>());
                    _threads.emplace_back(std::thread());
                }
            } else {
                _services.emplace_back(std::make_shared<asio::io_service>());
                for (int thread = 0; thread < threads; ++thread)
                    _threads.emplace_back(std::thread());
                _strand = std::make_shared<asio::io_service::strand>(*_services[0]);
                _strand_required = true;
            }
        }

        Service::Service(const std::shared_ptr<asio::io_service> &service, bool strands) :
            _strand_required(strands), _polling(false), _started(false), _round_robin_index(0) {
            if (service == nullptr)
                throw AsioUtils::ArgumentException("Asio IO service is invalid!");
            _services.emplace_back(service);
            if (_strand_required)
                _strand = std::make_shared<asio::io_service::strand>(*_services[0]);
        }

        bool Service::Start(bool polling) {
            if (IsStarted())
                return false;
            _polling = polling;
            _round_robin_index = 0;
            auto self(this->shared_from_this());
            auto start_handler = [this, self]() {
                if (IsStarted())
                    return;
                _started = true;
                onStarted();
            };
            if (_strand_required)
                _strand->post(start_handler);
            else
                _services[0]->post(start_handler);
            for (size_t thread = 0; thread < _threads.size(); ++thread)
                _threads[thread] = AsioUtils::Thread::Start(
                        [this, self, thread]() { ServiceThread(self, _services[thread % _services.size()]); });
            while (!IsStarted())
                AsioUtils::Thread::Yield();
            return true;
        }

        bool Service::Stop() {
            if (!IsStarted())
                return false;
            auto self(this->shared_from_this());
            auto stop_handler = [this, self]() {
                if (!IsStarted())
                    return;
                for (auto &service: _services)
                    service->stop();
                _started = false;
                onStopped();
            };
            if (_strand_required)
                _strand->post(stop_handler);
            else
                _services[0]->post(stop_handler);
            for (auto &thread: _threads)
                thread.join();
            _polling = false;
            while (IsStarted())
                AsioUtils::Thread::Yield();
            return true;
        }

        bool Service::Restart() {
            bool polling = IsPolling();
            if (!Stop())
                return false;
            for (size_t service = 0; service < _services.size(); ++service)
                _services[service] = std::make_shared<asio::io_service>();
            if (_strand_required)
                _strand = std::make_shared<asio::io_service::strand>(*_services[0]);
            return Start(polling);
        }

        void Service::ServiceThread(const std::shared_ptr<Service> &service,
                                    const std::shared_ptr<asio::io_service> &io_service) {
            bool polling = service->IsPolling();
            service->onThreadInitialize();
            try {
                asio::io_service::work work(*io_service);
                do {
                    try {
                        if (polling) {
                            io_service->poll();
                            service->onIdle();
                        } else {
                            io_service->run();
                            break;
                        }
                    } catch (const asio::system_error &ex) {
                        std::error_code ec = ex.code();
                        if (ec == asio::error::not_connected)
                            continue;
                        throw;
                    }
                } while (service->IsStarted());
            } catch (const asio::system_error &ex) {
                service->SendError(ex.code());
            } catch (const std::exception &ex) {
                AsioUtils::fatal(AsioUtils::SourceLocation(__FILE__, __LINE__), AsioUtils::StackTrace(), ex);
            } catch (...) {

                AsioUtils::fatal(AsioUtils::SourceLocation(__FILE__, __LINE__), AsioUtils::StackTrace(),
                                 "Asio service thread terminated!");
            }
            service->onThreadCleanup();
        }

        void Service::SendError(std::error_code ec) { onError(ec.value(), ec.category().name(), ec.message()); }

    }
}
