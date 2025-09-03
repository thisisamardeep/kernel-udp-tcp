#ifndef ASIOWRAPPER_ASIO_UDP_CLIENT_H
#define ASIOWRAPPER_ASIO_UDP_CLIENT_H

#include "timespan.h"
#include "uuid.h"
#include "udp_resolver.h"

#include <mutex>
#include <vector>

namespace AsioWrapper {
namespace Asio {

class UDPClient : public std::enable_shared_from_this<UDPClient>
{
public:
    UDPClient(const std::shared_ptr<Service>& service, const std::string& address, int port);
    UDPClient(const std::shared_ptr<Service>& service, const std::string& address, const std::string& scheme);
    UDPClient(const std::shared_ptr<Service>& service, const asio::ip::udp::endpoint& endpoint);
    UDPClient(const UDPClient&) = delete;
    UDPClient(UDPClient&&) = delete;
    virtual ~UDPClient() = default;

    UDPClient& operator=(const UDPClient&) = delete;
    UDPClient& operator=(UDPClient&&) = delete;

    const AsioUtils::UUID& id() const noexcept { return _id; }

    std::shared_ptr<Service>& service() noexcept { return _service; }
    std::shared_ptr<asio::io_service>& io_service() noexcept { return _io_service; }
    asio::io_service::strand& strand() noexcept { return _strand; }
    asio::ip::udp::endpoint& endpoint() noexcept { return _endpoint; }
    asio::ip::udp::socket& socket() noexcept { return _socket; }

    const std::string& address() const noexcept { return _address; }
    const std::string& scheme() const noexcept { return _scheme; }
    int port() const noexcept { return _port; }

    uint64_t bytes_pending() const noexcept { return _bytes_sending; }
    uint64_t bytes_sent() const noexcept { return _bytes_sent; }
    uint64_t bytes_received() const noexcept { return _bytes_received; }
    uint64_t datagrams_sent() const noexcept { return _datagrams_sent; }
    uint64_t datagrams_received() const noexcept { return _datagrams_received; }

    bool option_reuse_address() const noexcept { return _option_reuse_address; }
    bool option_reuse_port() const noexcept { return _option_reuse_port; }
    bool option_multicast() const noexcept { return _option_multicast; }
    size_t option_receive_buffer_limit() const noexcept { return _receive_buffer_limit; }
    size_t option_receive_buffer_size() const;
    size_t option_send_buffer_limit() const noexcept { return _send_buffer_limit; }
    size_t option_send_buffer_size() const;

    bool IsConnected() const noexcept { return _connected; }

    virtual bool Connect();
    virtual bool Connect(const std::shared_ptr<UDPResolver>& resolver);
    virtual bool Disconnect() { return DisconnectInternal(); }
    virtual bool Reconnect();

    virtual bool ConnectAsync();
    virtual bool ConnectAsync(const std::shared_ptr<UDPResolver>& resolver);
    virtual bool DisconnectAsync() { return DisconnectInternalAsync(false); }
    virtual bool ReconnectAsync();

    virtual void JoinMulticastGroup(const std::string& address);
    virtual void LeaveMulticastGroup(const std::string& address);

    virtual void JoinMulticastGroupAsync(const std::string& address);
    virtual void LeaveMulticastGroupAsync(const std::string& address);

    virtual size_t Send(const void* buffer, size_t size);
    virtual size_t Send(std::string_view text) { return Send(text.data(), text.size()); }
    virtual size_t Send(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size);
    virtual size_t Send(const asio::ip::udp::endpoint& endpoint, std::string_view text) { return Send(endpoint, text.data(), text.size()); }

    virtual size_t Send(const void* buffer, size_t size, const AsioUtils::Timespan& timeout);
    virtual size_t Send(std::string_view text, const AsioUtils::Timespan& timeout) { return Send(text.data(), text.size(), timeout); }
    virtual size_t Send(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size, const AsioUtils::Timespan& timeout);
    virtual size_t Send(const asio::ip::udp::endpoint& endpoint, std::string_view text, const AsioUtils::Timespan& timeout) { return Send(endpoint, text.data(), text.size(), timeout); }

    virtual bool SendAsync(const void* buffer, size_t size);
    virtual bool SendAsync(std::string_view text) { return SendAsync(text.data(), text.size()); }
    virtual bool SendAsync(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size);
    virtual bool SendAsync(const asio::ip::udp::endpoint& endpoint, std::string_view text) { return SendAsync(endpoint, text.data(), text.size()); }

    virtual size_t Receive(asio::ip::udp::endpoint& endpoint, void* buffer, size_t size);
    virtual std::string Receive(asio::ip::udp::endpoint& endpoint, size_t size);

    virtual size_t Receive(asio::ip::udp::endpoint& endpoint, void* buffer, size_t size, const AsioUtils::Timespan& timeout);
    virtual std::string Receive(asio::ip::udp::endpoint& endpoint, size_t size, const AsioUtils::Timespan& timeout);

    virtual void ReceiveAsync();

    void SetupReuseAddress(bool enable) noexcept { _option_reuse_address = enable; }
    void SetupReusePort(bool enable) noexcept { _option_reuse_port = enable; }
    void SetupMulticast(bool enable) noexcept { _option_reuse_address = enable; _option_multicast = enable; }

protected:
    virtual void onConnected() {}
    virtual void onDisconnected() {}

    virtual void onJoinedMulticastGroup(const std::string& address) {}
    virtual void onLeftMulticastGroup(const std::string& address) {}

    virtual void onReceived(const asio::ip::udp::endpoint& endpoint, const void* buffer, size_t size) {}
    virtual void onSent(const asio::ip::udp::endpoint& endpoint, size_t sent) {}

    virtual void onError(int error, const std::string& category, const std::string& message) {}

private:
    AsioUtils::UUID _id;
    std::shared_ptr<Service> _service;
    std::shared_ptr<asio::io_service> _io_service;
    asio::io_service::strand _strand;
    bool _strand_required;
    std::string _address;
    std::string _scheme;
    int _port;
    asio::ip::udp::endpoint _endpoint;
    asio::ip::udp::socket _socket;
    std::atomic<bool> _resolving;
    std::atomic<bool> _connected;
    uint64_t _bytes_sending;
    uint64_t _bytes_sent;
    uint64_t _bytes_received;
    uint64_t _datagrams_sent;
    uint64_t _datagrams_received;
    asio::ip::udp::endpoint _receive_endpoint;
    asio::ip::udp::endpoint _send_endpoint;
    bool _receiving;
    size_t _receive_buffer_limit{0};
    std::vector<uint8_t> _receive_buffer;
    HandlerStorage _receive_storage;
    bool _sending;
    size_t _send_buffer_limit{0};
    std::vector<uint8_t> _send_buffer;
    HandlerStorage _send_storage;
    bool _option_reuse_address;
    bool _option_reuse_port;
    bool _option_multicast;

    bool DisconnectInternal();
    bool DisconnectInternalAsync(bool dispatch);

    void TryReceive();


    void SendError(std::error_code ec);
};

}
}

#endif