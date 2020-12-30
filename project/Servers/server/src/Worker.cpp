#include "Worker.h"
#include <boost/log/trivial.hpp>

Worker::Worker(std::shared_ptr<CommandManager> manager,
               std::shared_ptr<NetworkServer> server)
    : _manager(std::move(manager)),
      _networkServer(std::move(server)) {
  BOOST_LOG_TRIVIAL(debug) << "Worker: create worker";
}

Worker::~Worker() {
  BOOST_LOG_TRIVIAL(debug) << "Worker: delete worker";
}

void Worker::Run() {
  BOOST_LOG_TRIVIAL(info) << "Worker: Run";
  listening();
}

void Worker::listening() {
  while (true) {
    auto request = _networkServer->GetRequest();
    if (request != nullptr) {
      BOOST_LOG_TRIVIAL(info) << "Worker: new request";
      onConnect(request);
    } else {
      BOOST_LOG_TRIVIAL(debug) << "Worker: sleep";
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
  }
}

void Worker::onConnect(const std::shared_ptr<std::pair<std::shared_ptr<UserSession>, pt::ptree>> &request) {
  BOOST_LOG_TRIVIAL(info) << "Worker: start working with user";

  auto response = _manager->GetRequest(request->second);
  _networkServer->PutResponse(std::make_shared<std::pair<std::shared_ptr<UserSession>,
                                                         pt::ptree>>(request->first, *response));

  BOOST_LOG_TRIVIAL(info) << "Worker: stop working with user";
}
