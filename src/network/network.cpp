#include "network.h"

#include <system_error>
#include <netinet/in.h>

#include "socket.h"

void Network::startListening()
{
    server_fd = Socket();
    server_fd.bindSocket();
    server_fd.startListening();
}

void Network::handleConnection(const Callback& callback) const
{
    while (true)
    {
        sockaddr_in client_address{};
        socklen_t client_addrlen = sizeof(client_address);
        int new_socket = accept(server_fd.getFd(), reinterpret_cast<sockaddr*>(&client_address), &client_addrlen);
        if (new_socket < 0)
        {
            throw std::system_error(errno, std::generic_category(), "Accept failed");
        }

        Socket client_socket(new_socket);
        callback(client_socket);
    }
}
