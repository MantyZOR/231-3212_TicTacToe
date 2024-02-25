#include <iostream>
#include <string>
#include <string_view>
#include <array>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <system_error>
#include <cerrno>

class Socket {
public:
    Socket() {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            throw std::system_error(errno, std::generic_category(), "Failed to create socket");
        }
    }

    ~Socket() {
        if (server_fd != -1) {
            close(server_fd);
        }
    }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket&& other) noexcept : server_fd(other.server_fd) {
        other.server_fd = -1;
    }

    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) {
            close(server_fd);
            server_fd = other.server_fd;
            other.server_fd = -1;
        }
        return *this;
    }

    int getFd() const { return server_fd; }

private:
    int server_fd = -1;
};

int main() {
    try {
        Socket server_fd;

        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(8082);

        if (bind(server_fd.getFd(), reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
            throw std::system_error(errno, std::generic_category(), "Bind failed");
        }

        std::array<char, INET_ADDRSTRLEN> myIP{};
        inet_ntop(AF_INET, &address.sin_addr, myIP.data(), myIP.size());
        int myPort = ntohs(address.sin_port);
        std::cout << "Server listening on IP: " << myIP.data() << " Port: " << myPort << std::endl;

        if (listen(server_fd.getFd(), 10) < 0) {
            throw std::system_error(errno, std::generic_category(), "Listen failed");
        }

        while (true) {
            std::cout << "\n[info] Waiting for new connection\n\n";
            sockaddr_in client_address{};
            socklen_t client_addrlen = sizeof(client_address);
            int new_socket = accept(server_fd.getFd(), reinterpret_cast<sockaddr*>(&client_address), &client_addrlen);
            if (new_socket < 0) {
                throw std::system_error(errno, std::generic_category(), "Accept failed");
            }

            std::array<char, 1024> buffer{};
            long valread = read(new_socket, buffer.data(), buffer.size());
            if (valread < 0) {
                close(new_socket);
                continue;
            }

            std::string_view hello = "Hello from server";
            write(new_socket, hello.data(), hello.size());
            std::cout << "[info] Hello Message sent \n";
            close(new_socket);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
