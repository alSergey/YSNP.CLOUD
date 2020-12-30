#pragma once

#include <libpq-fe.h>
#include <string>
#include "PostgresExceptions.h"

class PostgresSQLDB {
 public:
  virtual ~PostgresSQLDB();
  void Connect();

 protected:
  explicit PostgresSQLDB(std::string_view info);
  PGconn *_conn{};
  std::string _connInfo;
  void pqExec(const std::string &query, PostgresExceptions exceptions);

 private:
  void close();
};
