#pragma once

#include <functional>
#include <utility>
#include <memory>
#include <string>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include "SafeQueue.h"
#include "UserSession.h"

namespace pt = boost::property_tree;

class ClientWorker {
 public:
  ClientWorker() = default;
  ~ClientWorker() = default;

  void RunClientWorker();
  std::shared_ptr<std::pair<std::shared_ptr<UserSession>, pt::ptree> > SendRequestToServer();
  void GetResponseFromServer(const std::shared_ptr<std::pair<std::shared_ptr<UserSession>,
                                                             pt::ptree> > &response);
  void PutConnection(const std::shared_ptr<UserSession> &user);

 private:
  void sendResponse();
  void getRequest(const std::shared_ptr<UserSession> &user);
  [[noreturn]] void workerSendResponses();
  [[noreturn]] void workerGetRequests();

 private:
  SafeQueue<std::shared_ptr<UserSession>> _usersConnections;
  SafeQueue<std::shared_ptr<std::pair<std::shared_ptr<UserSession>, pt::ptree> > > _request;
  SafeQueue<std::shared_ptr<std::pair<std::shared_ptr<UserSession>, pt::ptree> > > _response;
};
