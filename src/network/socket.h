#ifndef SOCKET_H
#define SOCKET_H
#include <string>
#include <netinet/in.h>

class Socket
{
public:
    Socket();
    Socket(int fd);
    ~Socket();

    Socket(const Socket&)            = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    void bindSocket();
    void startListening() const;

    std::string readFromSocket() const;
    void        writeToSocket(const std::string& message) const;

    [[nodiscard]] int getFd() const;

private:
    int         server_fd = -1;
    sockaddr_in address{};
};

#endif //SOCKET_H
