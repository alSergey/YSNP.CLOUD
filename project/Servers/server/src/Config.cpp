#include "Config.h"
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

Config::Config() {
  BOOST_LOG_TRIVIAL(debug) << "Config: create config";
}

Config::~Config() {
  BOOST_LOG_TRIVIAL(debug) << "Config: delete config";
}

Config &Config::GetInstance() {
  BOOST_LOG_TRIVIAL(debug) << "Config: GetInstance";
  static Config instance;
  return instance;
}

ServerConfig &Config::GetServerConfig() {
  BOOST_LOG_TRIVIAL(debug) << "Config: GetServerConfig";
  return _serverConfig;
}

NetworkConfig &Config::GetNetworkConfig() {
  BOOST_LOG_TRIVIAL(debug) << "Config: GetNetworkConfig";
  return _networkConfig;
}

ClientWorkerConfig &Config::GetClientWorkerConfig() {
  BOOST_LOG_TRIVIAL(debug) << "Config: GetClientWorkerConfig";
  return _clientWorkerConfig;
}

int Config::Run(const std::string &path) {
  BOOST_LOG_TRIVIAL(debug) << "Config: Run";
  pt::ptree root;

  try {
    pt::read_json(path, root);
  } catch (pt::ptree_error &er) {
    BOOST_LOG_TRIVIAL(fatal) << "Config: " << er.what();
    return EXIT_FAILURE;
  }

  try {
    parseLogConfig(root.get_child("log"));
  } catch (pt::ptree_error &er) {
    BOOST_LOG_TRIVIAL(fatal) << "Config: " << er.what();
    return EXIT_FAILURE;
  }

  try {
    parseStorageConfig(root.get_child("serverConfig"));
  } catch (pt::ptree_error &er) {
    BOOST_LOG_TRIVIAL(fatal) << "Config: " << er.what();
    return EXIT_FAILURE;
  }

  try {
    parseNetworkConfig(root.get_child("networkConfig"));
  } catch (pt::ptree_error &er) {
    BOOST_LOG_TRIVIAL(fatal) << "Config: " << er.what();
    return EXIT_FAILURE;
  }

  try {
    parseClientWorkerConfig(root.get_child("clientWorkerConfig"));
  } catch (pt::ptree_error &er) {
    BOOST_LOG_TRIVIAL(fatal) << "Config: " << er.what();
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void Config::parseLogConfig(const pt::ptree &log) {
  BOOST_LOG_TRIVIAL(debug) << "Config: parseLogConfig";
  if (log.get<std::string>("state") == "release") {
    logging::core::get()->set_filter
        (
            logging::trivial::severity >= logging::trivial::info);
  }

  auto file = log.get<std::string>("file");
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

void Config::parseStorageConfig(const pt::ptree &storage) {
  BOOST_LOG_TRIVIAL(debug) << "Config: parseStorageConfig";
  _serverConfig.workersCount = storage.get<size_t>("workersCount");
}

void Config::parseNetworkConfig(const pt::ptree &network) {
  BOOST_LOG_TRIVIAL(debug) << "Config: parseNetworkConfig";
  _networkConfig.port = network.get<size_t>("port");
  _networkConfig.backlog = network.get<size_t>("backlog");
}

void Config::parseClientWorkerConfig(const pt::ptree &clientWorker) {
  BOOST_LOG_TRIVIAL(debug) << "Config: parseClientWorkerConfig";
  _clientWorkerConfig.requestWorkersCount = clientWorker.get<size_t>("requestWorkersCount");
  _clientWorkerConfig.responseWorkersCount = clientWorker.get<size_t>("responseWorkersCount");
}
