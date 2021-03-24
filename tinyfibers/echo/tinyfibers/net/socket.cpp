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

  if (!error || error == asio::error::eof) {
    return Ok(read_bytes);
  } else {
    return Fail(error);
  }
}

Result<size_t> Socket::Read(MutableBuffer buffer) {
  size_t total_read = 0;
  auto read_some_result =
      ReadSome(MutableBuffer(static_cast<char*>(buffer.data()) + total_read,
                             buffer.size() - total_read));

  while (read_some_result.IsOk() && read_some_result.ValueOrThrow() != 0 &&
         total_read < buffer.size()) {
    total_read += read_some_result.ValueOrThrow();
    read_some_result =
        ReadSome(MutableBuffer(static_cast<char*>(buffer.data()) + total_read,
                               buffer.size() - total_read));
  }

  if (read_some_result.IsOk()) {
    return Ok(total_read);
  } else {
    return read_some_result;
  }
}

Status Socket::Write(ConstBuffer buffer) {
  auto waitee = std::make_shared<ParkingLot>();
  asio::error_code error{};

  asio::async_write(socket_, buffer,
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
  asio::error_code error{};
  socket_.shutdown(asio::socket_base::shutdown_send);
  if (error) {
    return Fail(error);
  } else {
    return Ok();
  }
}

Socket::Socket(asio::ip::tcp::socket&& impl) : socket_(std::move(impl)) {
}

}  // namespace tinyfibers::net
