#include <echo/server.hpp>

#include <tinyfibers/api.hpp>
#include <tinyfibers/net/acceptor.hpp>
#include <tinyfibers/net/socket.hpp>
#include <cassert>
#include <memory>
#include <string>

using tinyfibers::Spawn;
using tinyfibers::net::Acceptor;
using tinyfibers::net::Socket;

namespace echo {
enum { BUFFER_SIZE = 4096, BACKLOG = 1024 };

void HandleClient(Socket socket) {
  char buffer[BUFFER_SIZE] = "";

  auto read_bytes = socket.ReadSome(asio::buffer(buffer, BUFFER_SIZE));
  while (read_bytes.IsOk() && read_bytes.ValueOrThrow() > 0) {
    socket.Write(asio::buffer(buffer, read_bytes.ValueOrThrow())).ExpectOk();
    read_bytes = socket.ReadSome(asio::buffer(buffer, BUFFER_SIZE));
  }
}

void ServeForever(uint16_t port) {
  Spawn([port]() {
    Acceptor acceptor{};
    acceptor.BindTo(port).ExpectOk();
    acceptor.Listen(BACKLOG).ExpectOk();

    while (true) {
      Socket new_conn = acceptor.Accept();

      Spawn([socket = std::move(new_conn)]() mutable {
        HandleClient(std::move(socket));
      }).Detach();
    }
  }).Join();
}

}  // namespace echo
