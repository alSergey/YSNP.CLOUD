#include "StorageServer.h"
#include "Config.h"
#include <iostream>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char **argv) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help,h", "produce help message");

  po::options_description configDesc("Config options REQUIRED");
  configDesc.add_options()
      ("input,i", po::value<std::string>(), "Config file");

  desc.add(configDesc);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return EXIT_SUCCESS;
  }

  if (!vm.count("input")) {
    std::cout << "Choice config file" << std::endl;
    return EXIT_FAILURE;
  }

  auto &config = Config::GetInstance();
  if (config.Run(vm["input"].as<std::string>()) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  StorageServer server;
  server.Run();

  return EXIT_SUCCESS;
}
