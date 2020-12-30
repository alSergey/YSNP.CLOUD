#pragma once

#include <functional>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

class UserSession
    : public std::enable_shared_from_this<UserSession> {
 public:
  explicit UserSession(boost::asio::io_service &io);
  boost::asio::ip::tcp::socket &Sock();
  pt::ptree GetRequest();
  void SendResponse(const pt::ptree &jsonSend);

 private:
  boost::asio::ip::tcp::socket _socket;
  pt::ptree _jsonReceive;
  void receiveRequest();
  void receiveResponse(const pt::ptree &jsonSend);
};
