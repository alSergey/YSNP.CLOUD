#include "ClientNetwork.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/log/trivial.hpp>

ClientNetwork::ClientNetwork()
    : _socket(_service) {
  BOOST_LOG_TRIVIAL(debug) << "ClientNetwork: create ClientNetwork";
}

void ClientNetwork::SendJSON(const pt::ptree &jsonRequest) {
  BOOST_LOG_TRIVIAL(debug) << "ClientNetwork: SendJSON";
  std::stringstream ss;
  try {
    pt::json_parser::write_json(ss, jsonRequest, false);
  } catch (pt::ptree_error &er) {
    BOOST_LOG_TRIVIAL(error) << "ClientNetwork: error read JSON, while sending: " << er.what();
    throw SendToServerParseJsonException(er.what());
  }

  boost::system::error_code ec;
  boost::asio::write(_socket, boost::asio::buffer(ss.str() + '\n'), ec);
  if (ec) {
    BOOST_LOG_TRIVIAL(error) << "ClientNetwork: error send to server: " << ec.message().c_str();
    throw SendToServerException(ec.message());
  }
}

pt::ptree ClientNetwork::ReceiveJSON() {
  BOOST_LOG_TRIVIAL(debug) << "ClientNetwork: ReceiveJSON";
  boost::system::error_code ec;
  boost::asio::streambuf buf;
  boost::asio::read(_socket, buf, boost::asio::transfer_all(), ec);
  if (ec && ec != boost::asio::error::eof) {
    BOOST_LOG_TRIVIAL(error) << "ClientNetwork: error read from server: " << ec.message().c_str();
  }

  std::stringstream ss(boost::asio::buffer_cast<const char *>(buf.data()));
  pt::ptree jsonResponse;
  try {
    pt::read_json(ss, jsonResponse);
  } catch (pt::ptree_error &er) {
    BOOST_LOG_TRIVIAL(error) << "ClientNetwork: error write JSON to ptree, while receiving: " << er.what();
    throw ReadFromServerParseJsonException(er.what());
  }
  return jsonResponse;
}

void ClientNetwork::Connect(const std::string &localhost, int port) {
  BOOST_LOG_TRIVIAL(debug) << "ClientNetwork: connect to server: " << localhost << " " << port;
  boost::asio::ip::tcp::resolver resolver(_service);
  std::string sport = std::to_string(port);

  boost::system::error_code ec;
  boost::asio::connect(_socket, resolver.resolve({localhost, sport}), ec);
  if (ec) {
    BOOST_LOG_TRIVIAL(error) << "ClientNetwork: error connect to server: " << ec.message().c_str();
    throw ConnectServerException(ec.message());
  }
}

void ClientNetwork::Disconnect() {
  BOOST_LOG_TRIVIAL(debug) << "ClientNetwork: Disconnect";
  _socket.close();
}

ClientNetwork::~ClientNetwork() {
  BOOST_LOG_TRIVIAL(debug) << "ClientNetwork: delete ClientNetwork";
  Disconnect();
}
