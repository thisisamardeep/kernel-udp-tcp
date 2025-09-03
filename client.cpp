

#include "asio/udp_client.h"
#include "include/server/thread.h"

#include <atomic>
#include <iostream>

class AsioService : public AsioWrapper::Asio::Service
{
public:
    using AsioWrapper::Asio::Service::Service;

protected:
    void onError(int error, const std::string& category, const std::string& message) override
    {
        std::cout << "Asio service caught an error with code " << error << " and category '" << category << "': " << message << std::endl;
    }
};


class MulticastClient : public AsioWrapper::Asio::UDPClient
{
public:
    MulticastClient(const std::shared_ptr<AsioWrapper::Asio::Service>& service, const std::string& address, const std::string& multicast, int port)
        : AsioWrapper::Asio::UDPClient(service, address, port),
          _multicast(multicast)
    {
    }

    void DisconnectAndStop()
    {
        _stop = true;
        DisconnectAsync();
        while (IsConnected())
            AsioUtils::Thread::Yield();
    }

protected:
    void onConnected() override
    {
        std::cout << "Multicast UDP client connected a new session with Id " << id() << std::endl;

        JoinMulticastGroup(_multicast);

        ReceiveAsync();
    }

    void onDisconnected() override
    {
        std::cout << "Multicast UDP client disconnected a session with Id " << id() << std::endl;

        AsioUtils::Thread::Sleep(1000);

        if (!_stop)
            ConnectAsync();
    }

    void onReceived(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size) override
    {
        std::cout << "Incoming: " << std::string((const char*)buffer, size) << std::endl;

        ReceiveAsync();
    }

    void onError(int error, const std::string& category, const std::string& message) override
    {
        std::cout << "Multicast UDP client caught an error with code " << error << " and category '" << category << "': " << message << std::endl;
    }

private:
    std::atomic<bool> _stop{false};
    std::string _multicast;
};

int main(int argc, char** argv)
{
    std::string listen_address = "0.0.0.0";
    if (argc > 1)
        listen_address = argv[1];

    std::string multicast_address = "239.255.0.1";
    if (argc > 2)
        multicast_address = argv[2];

    int multicast_port = 3334;
    if (argc > 3)
        multicast_port = std::atoi(argv[3]);

    std::cout << "UDP listen address: " << listen_address << std::endl;
    std::cout << "UDP multicast address: " << multicast_address << std::endl;
    std::cout << "UDP multicast port: " << multicast_port << std::endl;

    std::cout << std::endl;

    auto service = std::make_shared<AsioService>();

    service->Start();

    auto client = std::make_shared<MulticastClient>(service, listen_address, multicast_address, multicast_port);
    client->SetupMulticast(true);

    std::cout << "Client connecting...";
    client->ConnectAsync();
    std::cout << "Done!" << std::endl;

    std::cout << "Press Enter to stop the client or '!' to reconnect the client..." << std::endl;

    std::string line;
    while (getline(std::cin, line))
    {
        if (line.empty())
            break;

        if (line == "!")
        {
            std::cout << "Client reconnecting...";
            client->IsConnected() ? client->ReconnectAsync() : client->ConnectAsync();
            std::cout << "Done!" << std::endl;
            continue;
        }
    }

    std::cout << "Client disconnecting...";
    client->DisconnectAndStop();
    std::cout << "Done!" << std::endl;

    service->Stop();

    return 0;
}
