#include "NetworkServer.h"
#include <boost/log/trivial.hpp>

NetworkServer::NetworkServer(size_t port, size_t backlog)
    : _acceptor(_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
  BOOST_LOG_TRIVIAL(info) << "NetworkSever: listen on port " << port << ", backlog: " << backlog;

  _acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  _acceptor.listen(backlog);
}

void NetworkServer::StartServer() {
  BOOST_LOG_TRIVIAL(debug) << "NetworkSever: StartServer";
  std::thread threadServerRunning(&ClientWorker::RunClientWorker, &_queue);
  threadServerRunning.detach();

  startAccept();
  run();
}

void NetworkServer::run() {
  BOOST_LOG_TRIVIAL(debug) << "NetworkSever: run service";
  boost::system::error_code ec;
  _service.run(ec);
  if (ec) {
    BOOST_LOG_TRIVIAL(fatal) << "NetworkSever: error run service: " << ec.message().c_str();
    throw ErrorRunServerException(ec.message());
  }
}

void NetworkServer::startAccept() {
  BOOST_LOG_TRIVIAL(debug) << "NetworkSever: startAccept";
  auto user = std::make_shared<UserSession>(_service);
  _acceptor.async_accept(user->Sock(),
                         boost::bind(&NetworkServer::onAccept, this, user,
                                     boost::asio::placeholders::error));
}

void NetworkServer::onAccept(const std::shared_ptr<UserSession> &user, const boost::system::error_code &ec) {
  BOOST_LOG_TRIVIAL(info) << "NetworkSever: accept user";

  if (ec) {
    BOOST_LOG_TRIVIAL(error) << "NetworkSever: error accept user" << ec.message().c_str();
    return;
  }
  _queue.PutConnection(user);
  startAccept();
}

void NetworkServer::PutResponse(
    const std::shared_ptr<std::pair<std::shared_ptr<UserSession>, boost::property_tree::ptree> > &response) {
  BOOST_LOG_TRIVIAL(debug) << "NetworkSever: PutResponse";
  _queue.GetResponseFromServer(response);
}

std::shared_ptr<std::pair<std::shared_ptr<UserSession>,
                          boost::property_tree::ptree> > NetworkServer::GetRequest() {
  BOOST_LOG_TRIVIAL(debug) << "NetworkSever: GetRequest";
  return _queue.SendRequestToServer();
}



