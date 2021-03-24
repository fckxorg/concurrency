#include <tinyfibers/net/acceptor.hpp>

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

Acceptor::Acceptor() : acceptor_(Scheduler::GetIOContext()) {
}

Status Acceptor::BindTo(uint16_t port) {
  asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
  acceptor_.open(endpoint.protocol());

  asio::error_code error{};
  acceptor_.bind(endpoint, error);

  if (error) {
    return Fail(error);
  } else {
    return Ok();
  }
}

Result<uint16_t> Acceptor::BindToAvailablePort() {
  auto bind_result = BindTo(0);

  if (bind_result.IsOk()) {
    uint16_t choosen_port = GetPort();
    return Ok(std::move(choosen_port));
  } else {
    return Fail(bind_result.GetErrorCode());
  }
}

Status Acceptor::Listen(size_t backlog) {
  asio::error_code error{};

  acceptor_.listen(backlog, error);

  if (error) {
    return Fail(error);
  } else {
    return Ok();
  }
}

Result<Socket> Acceptor::Accept() {
  asio::io_context& io = Scheduler::GetIOContext();
  asio::ip::tcp::socket new_client{io};

  auto waitee = std::make_shared<ParkingLot>();

  asio::error_code error{};

  acceptor_.async_accept(new_client,
                         [waitee, &error](const asio::error_code& asio_error) {
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

uint16_t Acceptor::GetPort() const {
  return acceptor_.local_endpoint().port();
}

}  // namespace tinyfibers::net
