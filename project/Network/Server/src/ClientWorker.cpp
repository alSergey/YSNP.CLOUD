#include "ClientWorker.h"
#include <boost/log/trivial.hpp>

void ClientWorker::PutConnection(const std::shared_ptr<UserSession> &user) {
  BOOST_LOG_TRIVIAL(debug) << "ClientWorker: PutConnection";
  _usersConnections.Push(user);
}

// функция отдачи запроса воркеру
std::shared_ptr<std::pair<std::shared_ptr<UserSession>,
                          pt::ptree> > ClientWorker::SendRequestToServer() {
  BOOST_LOG_TRIVIAL(debug) << "ClientWorker: SendRequestToServer";
  if (!_request.Empty()) {
    return _request.Pop();
  }
  return nullptr;
}

// функция получения ответа от воркера
void ClientWorker::GetResponseFromServer(
    const std::shared_ptr<std::pair<std::shared_ptr<UserSession>, pt::ptree> > &response) {
  BOOST_LOG_TRIVIAL(debug) << "ClientWorker: GetResponseFromServer";
  _response.Push(response);
}

// функция получения запроса от пользователя
void ClientWorker::getRequest(const std::shared_ptr<UserSession> &user) {
  BOOST_LOG_TRIVIAL(debug) << "ClientWorker: getRequest";
  pt::ptree userRequest = user->GetRequest();

  if (!userRequest.empty()) {
    auto pairRequest = std::make_shared<std::pair<std::shared_ptr<UserSession>, pt::ptree>>(
        std::make_pair(user, userRequest));

    BOOST_LOG_TRIVIAL(info) << "ClientWorker: get new request from user";
    _request.Push(pairRequest);
  }
}

// функция отправки ответа пользователю
void ClientWorker::sendResponse() {
  BOOST_LOG_TRIVIAL(debug) << "ClientWorker: sendResponse";
  auto pairResponse = _response.Pop();
  if (pairResponse->first->Sock().is_open()) {
    BOOST_LOG_TRIVIAL(info) << "ClientWorker: send new response to user";
    pairResponse->first->SendResponse(pairResponse->second);
  } else {
    BOOST_LOG_TRIVIAL(error) << "ClientWorker: lost connection to user while sending response";
  }
}

// воркер получения запросов пользователя
[[noreturn]] void ClientWorker::workerGetRequests() {
  BOOST_LOG_TRIVIAL(debug) << "ClientWorker: workerGetRequests";
  while (true) {
    if (_usersConnections.Empty()) {
      BOOST_LOG_TRIVIAL(debug) << "ClientWorker: sleep request";
      std::this_thread::sleep_for(std::chrono::seconds(2));
    } else {
      BOOST_LOG_TRIVIAL(info) << "ClientWorker: get user connection";
      if (!_usersConnections.Empty()) {
        getRequest((_usersConnections.Pop()));
      }
    }
  }
}

// воркер отправки ответа пользователям
[[noreturn]] void ClientWorker::workerSendResponses() {
  BOOST_LOG_TRIVIAL(debug) << "ClientWorker: workerSendResponses";
  while (true) {
    if (_response.Empty()) {
      BOOST_LOG_TRIVIAL(debug) << "ClientWorker: sleep response";
      std::this_thread::sleep_for(std::chrono::seconds(2));
    } else {
      BOOST_LOG_TRIVIAL(info) << "ClientWorker: send response to user";
      if (!_response.Empty()) {
        sendResponse();
      }
    }
  }
}

void ClientWorker::RunClientWorker() {
  BOOST_LOG_TRIVIAL(debug) << "ClientWorker: run client network";

//  auto &config = Config::GetInstance();

  size_t requestWorkersCount = 1;
  std::vector<std::thread> _workerGetRequests;
  _workerGetRequests.reserve(requestWorkersCount);
  for (int i = 0; i < requestWorkersCount; ++i) {
    _workerGetRequests.emplace_back([this] { workerGetRequests(); });
    BOOST_LOG_TRIVIAL(info) << "ClientWorker: add new worker Get Requests with id = " << _workerGetRequests[i].get_id();
  }

  size_t responseWorkersCount = 1;
  std::vector<std::thread> _workerPutResponses;
  _workerPutResponses.reserve(responseWorkersCount);
  for (int i = 0; i < responseWorkersCount; ++i) {
    _workerPutResponses.emplace_back([this] { workerSendResponses(); });
    BOOST_LOG_TRIVIAL(info) << "ClientWorker: add new worker Put Responses with id = "
                            << _workerPutResponses[i].get_id();
  }

  for (auto &thread : _workerGetRequests) {
    thread.join();
    BOOST_LOG_TRIVIAL(info) << "ClientWorker: join worker Get Requests with id = " << thread.get_id();
  }
  _workerGetRequests.clear();

  for (auto &thread : _workerPutResponses) {
    thread.join();
    BOOST_LOG_TRIVIAL(info) << "ClientWorker: join worker Put Responses with id = " << thread.get_id();
  }
  _workerPutResponses.clear();
}
