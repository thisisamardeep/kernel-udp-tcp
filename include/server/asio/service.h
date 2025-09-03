

#ifndef ASIOWRAPPER_ASIO_SERVICE_H
#define ASIOWRAPPER_ASIO_SERVICE_H

#include "asio.h"
#include "memory.h"

#include "thread.h"

#include <atomic>
#include <cassert>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace AsioWrapper {
namespace Asio {



class Service : public std::enable_shared_from_this<Service>
{
public:

    explicit Service(int threads = 1, bool pool = false);

    explicit Service(const std::shared_ptr<asio::io_service>& service, bool strands = false);
    Service(const Service&) = delete;
    Service(Service&&) = delete;
    virtual ~Service() = default;

    Service& operator=(const Service&) = delete;
    Service& operator=(Service&&) = delete;

    size_t threads() const noexcept { return _threads.size(); }

    bool IsStrandRequired() const noexcept { return _strand_required; }
    bool IsPolling() const noexcept { return _polling; }
    bool IsStarted() const noexcept { return _started; }


    virtual bool Start(bool polling = false);

    virtual bool Stop();

    virtual bool Restart();

    virtual std::shared_ptr<asio::io_service>& GetAsioService() noexcept
    { return _services[++_round_robin_index % _services.size()]; }

    template <typename CompletionHandler>
    ASIO_INITFN_RESULT_TYPE(CompletionHandler, void()) Dispatch(ASIO_MOVE_ARG(CompletionHandler) handler)
    { if (_strand_required) return _strand->dispatch(handler); else return _services[0]->dispatch(handler); }


    template <typename CompletionHandler>
    ASIO_INITFN_RESULT_TYPE(CompletionHandler, void()) Post(ASIO_MOVE_ARG(CompletionHandler) handler)
    { if (_strand_required) return _strand->post(handler); else return _services[0]->post(handler); }

protected:

    virtual void onThreadInitialize() {}

    virtual void onThreadCleanup() {}

    virtual void onStarted() {}
    virtual void onStopped() {}

    virtual void onIdle() { AsioUtils::Thread::Yield(); }


    virtual void onError(int error, const std::string& category, const std::string& message) {}

private:
    std::vector<std::shared_ptr<asio::io_service>> _services;
    std::vector<std::thread> _threads;
    std::shared_ptr<asio::io_service::strand> _strand;
    std::atomic<bool> _strand_required;
    std::atomic<bool> _polling;
    std::atomic<bool> _started;
    std::atomic<size_t> _round_robin_index;

    static void ServiceThread(const std::shared_ptr<Service>& service, const std::shared_ptr<asio::io_service>& io_service);

    void SendError(std::error_code ec);
};


}
}

#endif
