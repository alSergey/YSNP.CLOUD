#include "resourcePath.h"
#include <boost/filesystem.hpp>

QString resourcePath() {
  return QString::fromStdString(boost::filesystem::current_path().string() + '/');
}