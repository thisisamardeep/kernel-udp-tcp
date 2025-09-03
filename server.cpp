#include <asio.hpp>
#include <iostream>

#include "asio/udp_server.h"
#include "asio/service.h"


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


class MulticastServer : public AsioWrapper::Asio::UDPServer
{
public:
    using AsioWrapper::Asio::UDPServer::UDPServer;

protected:
    void onError(int error, const std::string& category, const std::string& message) override
    {
        std::cout << "Multicast UDP server caught an error with code " << error << " and category '" << category << "': " << message << std::endl;
    }
};


int main(int argc, char** argv) {
    std::string multicast_address = "239.255.0.1";
    if (argc > 1)
        multicast_address = argv[1];

    int multicast_port = 3334;
    if (argc > 2)
        multicast_port = std::atoi(argv[2]);

    std::cout << "UDP multicast address: " << multicast_address << std::endl;
    std::cout << "UDP multicast port: " << multicast_port << std::endl;

    std::cout << std::endl;

    auto service = std::make_shared<AsioService>();

    std::cout << "Asio service starting...";
    service->Start();
    std::cout << "Done!" << std::endl;

    auto server = std::make_shared<MulticastServer>(service, 0);

    server->Start(multicast_address, multicast_port);
    std::cout << "Server Done!" << std::endl;

    std::cout << "Please type some message and press enter to broadcast some udpmulticast message" << std::endl;
    std::cout << "Press Enter to stop the server or '!' to restart the server..." << std::endl;

    std::string line;
    while (getline(std::cin, line))
    {
        if (line.empty())
            break;

        if (line == "!")
        {
            std::cout << "Server restarting...";
            server->Restart();
            std::cout << "Done!" << std::endl;
            continue;
        }

        // Multicast  message to all sessions
        line = "(udpmulticasttest) " + line;
        server->Multicast(line);
    }

    server->Stop();
    std::cout << "Server stopped!" << std::endl;

    service->Stop();
    return 0;
}