#include <tinyfibers/net/socket.hpp>

#include <tinyfibers/runtime/scheduler.hpp>
#include <tinyfibers/runtime/parking_lot.hpp>

using wheels::Result;
using wheels::Status;

using wheels::make_result::Fail;
using wheels::make_result::JustStatus;
using wheels::make_result::NotSupported;
using wheels::make_result::Ok;
using wheels::make_result::PropagateError;
using wheels::make_result::ToStatus;

namespace tinyfibers::net {

Result<Socket> Socket::ConnectTo(const std::string& host, uint16_t port) {
  asio::ip::tcp::endpoint endpoint{asio::ip::address::from_string(host), port};
  asio::ip::tcp::socket new_client{Scheduler::GetIOContext()};

  try {
    new_client.async_connect(endpoint,
                             [](const asio::error_code& /*error*/) {});
  } catch (asio::system_error error) {
    return Fail(error.code());
  }
  return Ok(Socket(std::move(new_client)));  // Your code goes here
}

Result<Socket> Socket::ConnectToLocal(uint16_t port) {
  return ConnectTo("127.0.0.1", port);
}

Result<size_t> Socket::ReadSome(MutableBuffer /*buffer*/) {
  return NotSupported();  // Your code goes here
}

Result<size_t> Socket::Read(MutableBuffer /*buffer*/) {
  return NotSupported();  // Your code goes here
}

Status Socket::Write(ConstBuffer /*buffer*/) {
  return NotSupported();  // Your code goes here
}

Status Socket::ShutdownWrite() {
  return NotSupported();  // Your code goes here
}

Socket::Socket(asio::ip::tcp::socket&& impl) : socket_(std::move(impl)) {
}

}  // namespace tinyfibers::net
