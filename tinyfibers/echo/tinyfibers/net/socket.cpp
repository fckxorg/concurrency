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

  auto waitee = std::make_shared<ParkingLot>();
  asio::error_code error{};

  new_client.async_connect(
      endpoint, [waitee, &error](const asio::error_code& asio_error) {
        error = std::move(asio_error);
        GetCurrentScheduler()->WakeOnAsyncComplete(waitee);
      });
  waitee->Park();
  if (error) {
    return Fail(error);
  } else {
    return Ok(Socket(std::move(new_client)));
  }
}

Result<Socket> Socket::ConnectToLocal(uint16_t port) {
  return ConnectTo("127.0.0.1", port);
}

Result<size_t> Socket::ReadSome(MutableBuffer buffer) {
  auto waitee = std::make_shared<ParkingLot>();
  asio::error_code error{};
  size_t read_bytes = 0;

  socket_.async_read_some(
      buffer, [waitee, &error, &read_bytes](const asio::error_code& asio_error,
                                            size_t bytes_transfered) {
        error = std::move(asio_error);
        read_bytes = bytes_transfered;
        GetCurrentScheduler()->WakeOnAsyncComplete(waitee);
      });

  waitee->Park();

  if (error) {
    return Fail(error);
  } else {
    return Ok(read_bytes);
  }
}

Result<size_t> Socket::Read(MutableBuffer buffer) {
  return ReadSome(buffer);
}

Status Socket::Write(ConstBuffer buffer) {
  auto waitee = std::make_shared<ParkingLot>();
  asio::error_code error{};

  socket_.async_write_some(buffer,
                           [waitee, &error](const asio::error_code asio_error,
                                            size_t /*bytes_transfered*/) {
                             error = std::move(asio_error);
                             GetCurrentScheduler()->WakeOnAsyncComplete(waitee);
                           });

  waitee->Park();

  if (error) {
    return Fail(error);
  } else {
    return Ok();
  }
}

Status Socket::ShutdownWrite() {
  return NotSupported();  // Your code goes here
}

Socket::Socket(asio::ip::tcp::socket&& impl) : socket_(std::move(impl)) {
}

}  // namespace tinyfibers::net
