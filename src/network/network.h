#ifndef NETWORK_H
#define NETWORK_H
#include "socket.h"
#include "functional"

class Network
{
public:
    static Network& getInstance()
    {
        static Network instance;
        return instance;
    }

    Network(Network const&)        = delete;
    void operator=(Network const&) = delete;

    void startListening();

    using Callback = std::function<void(Socket&)>;
    void handleConnection(const Callback& callback) const;

private:
    Network() = default;
    Socket server_fd;
};


#endif //NETWORK_H
