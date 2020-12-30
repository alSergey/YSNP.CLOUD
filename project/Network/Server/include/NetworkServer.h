#pragma once

#include <utility>
#include <memory>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include "UserSession.h"
#include "ClientWorker.h"
#include "ServerNetworkExceptions.h"

namespace pt = boost::property_tree;

class NetworkServer {
 public:
  explicit NetworkServer(size_t port = 5555, size_t backlog = 1024);
  void StartServer();
  void PutResponse(const std::shared_ptr<std::pair<std::shared_ptr<UserSession>,
                                                   pt::ptree> > &response);
  std::shared_ptr<std::pair<std::shared_ptr<UserSession>, pt::ptree> > GetRequest();

 private:
  void onAccept(const std::shared_ptr<UserSession> &user, const boost::system::error_code &ec);
  void startAccept();
  void run();

 private:
  ClientWorker _queue;
  boost::asio::io_service _service;
  boost::asio::ip::tcp::acceptor _acceptor;
};
