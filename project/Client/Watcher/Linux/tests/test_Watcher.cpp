#include "test.h"
#include <thread>

TEST(TestWatcher, RunTest) {
  AppleWatcher test_watch;
  std::string path = "/home/dantedoyl/watch";
  boost::filesystem::path fpath(path);

  auto alert = [&](Notification notification) {
    std::cout << "Event " << notification.event << " on " << notification.path << " at "
              << notification.time.time_since_epoch().count() << " was triggered." << std::endl;
  };

  std::thread thread([&]() { test_watch.Run(fpath, alert); });
  //std::this_thread::sleep_for(std::chrono::seconds(60));
  thread.join();
  if (auto ep = test_watch.GetLastException()) {
    try {
      std::rethrow_exception(ep);
    }
    catch (const std::exception &e) {
      std::cout << "exeption";
    }
  }




//    std::thread thread([&](){ test_watch.Run(fpath, alert); });
//    std::this_thread::sleep_for(std::chrono::seconds(60));
//    thread.join();
}