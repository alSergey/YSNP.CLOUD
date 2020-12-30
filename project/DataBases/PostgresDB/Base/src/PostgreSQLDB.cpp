#include "PostgreSQLDB.h"
#include "PostgresExceptions.h"
#include <boost/log/trivial.hpp>

void PostgresSQLDB::close() {
  PQfinish(_conn);
}

void PostgresSQLDB::Connect() {
  _conn = PQconnectdb(_connInfo.c_str());
  if (PQstatus(_conn) == CONNECTION_BAD) {
    BOOST_LOG_TRIVIAL(debug) << "PostgresSQLDB: Invalid to connect to DB" + std::string(PQerrorMessage(_conn));
    throw PostgresExceptions("invalid to connect to DB" + std::string(PQerrorMessage(_conn)));
  }
  BOOST_LOG_TRIVIAL(debug) << "PostgresSQLDB: Connect to DB";
}

PostgresSQLDB::~PostgresSQLDB() {
  close();
}

PostgresSQLDB::PostgresSQLDB(std::string_view info) : _connInfo(info) {}

void PostgresSQLDB::pqExec(const std::string &query, PostgresExceptions exceptions) {
  PGresult *res = PQexec(_conn, query.c_str());
  if (PQresultStatus(res) != PGRES_COMMAND_OK) {
    BOOST_LOG_TRIVIAL(debug) << "PostgresSQLDB: Faild to " + query;
    PQexec(_conn, "rollback to savepoint f_savepoint;");
    PQclear(res);
    throw exceptions;
  }
  PQclear(res);
}
