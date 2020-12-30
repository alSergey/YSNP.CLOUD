#pragma once

#include <string>
#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include "ClientNetworkExceptions.h"

namespace pt = boost::property_tree;

class ClientNetwork {
 public:
  ClientNetwork();
  ~ClientNetwork();
  void SendJSON(const pt::ptree &jsonRequest);
  pt::ptree ReceiveJSON();
  void Connect(const std::string &localhost = "localhost", int port = 5555);
  void Disconnect();
 private:
  boost::asio::io_service _service;
  boost::asio::ip::tcp::socket _socket;
};

