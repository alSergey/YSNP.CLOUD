#include "ClientConfig.h"
#include <iomanip>
#include <boost/property_tree/json_parser.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/support/date_time.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;

ClientConfig::ClientConfig() {
  BOOST_LOG_TRIVIAL(debug) << "ClientConfig: create config";
}

ClientConfig::~ClientConfig() {
  BOOST_LOG_TRIVIAL(debug) << "ClientConfig: delete config";
}

ClientConfig &ClientConfig::GetInstance() {
  BOOST_LOG_TRIVIAL(debug) << "ClientConfig: GetInstance";
  static ClientConfig instance;
  return instance;
}

void ClientConfig::Log(const std::string &state, const std::string &file) {
  BOOST_LOG_TRIVIAL(debug) << "ClientConfig: Log";
  if (state == "release") {
    logging::core::get()->set_filter
        (
            logging::trivial::severity >= logging::trivial::fatal);
  }

  if (!file.empty()) {
    logging::add_file_log
        (
            keywords::file_name = file + "_%N.log",
            keywords::rotation_size = 10 * 1024 * 1024,
            keywords::time_based_rotation =
                sinks::file::rotation_at_time_point(0, 0, 0),
            keywords::format =
                (
                    expr::stream
                        << std::hex << std::setw(8) << std::setfill('0') << expr::attr<unsigned int>("LineID")
                        << "\t"
                        << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f")
                        << "\t:"
                        << " <" << logging::trivial::severity << "> "
                        << "\t"
                        << expr::smessage));
  }
}
NetworkConfig ClientConfig::getStorageConfig() {
  BOOST_LOG_TRIVIAL(debug) << "ClientConfig: getStorageConfig";
  return NetworkConfig{.host = "35.224.63.74", .port = 5555};
}

NetworkConfig ClientConfig::getSyncConfig() {
  BOOST_LOG_TRIVIAL(debug) << "ClientConfig: getSyncConfig";
  return NetworkConfig{.host = "35.224.63.74", .port = 6666};
}
