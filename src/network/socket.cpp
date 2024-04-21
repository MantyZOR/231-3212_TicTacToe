#include "socket.h"

#include <array>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <system_error>
#include <cerrno>

Socket::Socket() {
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
	throw std::system_error(errno, std::generic_category(), "Failed to create socket");
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8083);
}

Socket::Socket(int fd) : server_fd(fd) {
  if (server_fd == -1) {
	throw std::system_error(errno, std::generic_category(), "Invalid socket file descriptor");
  }
}

Socket::~Socket() {
  if (server_fd != -1) {
	close(server_fd);
  }
}

Socket::Socket(Socket &&other) noexcept: server_fd(other.server_fd) {
  other.server_fd = -1;
}

Socket &Socket::operator=(Socket &&other) noexcept {
  if (this != &other) {
	close(server_fd);
	server_fd = other.server_fd;
	other.server_fd = -1;
  }
  return *this;
}

int Socket::getFd() const {
  return server_fd;
}

void Socket::bindSocket() {
  if (bind(server_fd, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0) {
	throw std::system_error(errno, std::generic_category(), "Bind failed");
  }
}

void Socket::startListening() const {
  if (listen(server_fd, 10) < 0) {
	throw std::system_error(errno, std::generic_category(), "Listen failed");
  }
}

std::string Socket::readFromSocket() const {
  std::array<char, 1024> buffer{};
  long valread = read(server_fd, buffer.data(), buffer.size());
  if (valread < 0) {
	throw std::system_error(errno, std::generic_category(), "Read failed");
  }
  return {buffer.data()};
}

void Socket::writeToSocket(const std::string &message) const {
  if (write(server_fd, message.data(), message.size()) < 0) {
	throw std::system_error(errno, std::generic_category(), "Write failed");
  }
}
