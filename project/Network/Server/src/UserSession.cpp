#include "UserSession.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/log/trivial.hpp>

UserSession::UserSession(boost::asio::io_service &io)
    : _socket(io) {
  BOOST_LOG_TRIVIAL(debug) << "UserSession: create UserSession";
}

pt::ptree UserSession::GetRequest() {
  BOOST_LOG_TRIVIAL(debug) << "UserSession: GetRequest";
  receiveRequest();
  return _jsonReceive;
}

void UserSession::SendResponse(const pt::ptree &jsonSend) {
  BOOST_LOG_TRIVIAL(debug) << "UserSession: SendResponse";
  receiveResponse(jsonSend);
}

boost::asio::ip::tcp::socket &UserSession::Sock() {
  BOOST_LOG_TRIVIAL(debug) << "UserSession: return Sock";
  return _socket;
}

void UserSession::receiveRequest() {
  BOOST_LOG_TRIVIAL(debug) << "UserSession: receiveRequest";
  boost::asio::streambuf buf;
  boost::system::error_code ec;
  boost::asio::read_until(_socket, buf, '\n', ec);
  if (ec) {
    BOOST_LOG_TRIVIAL(error) << "UserSession: error read from user length: " << ec.message().c_str();
    _socket.async_write_some(
        boost::asio::buffer(ec.message()),
        [self = shared_from_this()](const boost::system::error_code &ec,
                                    std::size_t) -> void {
          self->receiveRequest();
          if (ec)
            BOOST_LOG_TRIVIAL(error) << "UserSession: error send error-response to user: "
                                     << ec.message().c_str();
        });
    return;
  }

  std::stringstream ss(boost::asio::buffer_cast<const char *>(buf.data()));
  try {
    pt::read_json(ss, _jsonReceive);
  } catch (pt::ptree_error &er) {
    BOOST_LOG_TRIVIAL(error) << "UserSession: error write to ptree: " << er.what();
    return;
  }
}

void UserSession::receiveResponse(const pt::ptree &jsonSend) {
  BOOST_LOG_TRIVIAL(debug) << "UserSession: send response";
  std::stringstream ss;
  try {
    pt::json_parser::write_json(ss, jsonSend, false);
  } catch (pt::ptree_error &er) {
    BOOST_LOG_TRIVIAL(error) << "UserSession: error read from ptree: " << er.what();
    return;
  }

  boost::system::error_code ec;
  boost::asio::write(_socket, boost::asio::buffer(ss.str() + '\n'), ec);
  if (ec) {
    BOOST_LOG_TRIVIAL(error) << "UserSession: error send response to user: " << ec.message().c_str();
    return;
  }

//  _socket.async_write_some(
//      boost::asio::buffer(ss.str() + '\0'),
//      [self = shared_from_this()](const boost::system::error_code &ec,
//                                  std::size_t) -> void {
//        if (ec) {
//          BOOST_LOG_TRIVIAL(error) << "UserSession: error send response to user: " << ec.message().c_str();
//          return;
//        }
//      });
}
