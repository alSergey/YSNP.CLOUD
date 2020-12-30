#pragma once

#include <memory>
#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

class Command {
 public:
  virtual ~Command() = default;
  virtual std::shared_ptr<pt::ptree> Do() = 0;
};
