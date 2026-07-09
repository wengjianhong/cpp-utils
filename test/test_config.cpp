#include <cpputils/database/config.hpp>

#include <gtest/gtest.h>

TEST(DatabaseConfigTest, PostgreSqlTcpConnString) {
  cpp_utils::database::PostgreSqlConfig config;
  config.host = "10.0.0.1";
  config.port = 5433;
  config.user = "app";
  config.password = "secret";
  config.database_name = "mydb";
  config.ssl_mode = "require";
  config.connect_timeout = 3;

  const cpp_utils::database::ConnectionConfig opts(config);
  EXPECT_EQ(opts.database_type, cpp_utils::database::DatabaseType::kPostgreSql);
  EXPECT_EQ(opts.conn_string,
            "host=10.0.0.1 port=5433 dbname=mydb user=app password=secret sslmode=require connect_timeout=3 ");
}

TEST(DatabaseConfigTest, PostgreSqlUnixConnString) {
  cpp_utils::database::PostgreSqlConfig config;
  config.connection_type = cpp_utils::database::PostgreSqlConnectionType::kUnix;
  config.socket_path = "/var/run/postgresql";
  config.port = 5432;
  config.database_name = "nasdb";

  const cpp_utils::database::ConnectionConfig opts(config);
  EXPECT_EQ(opts.conn_string, "host=/var/run/postgresql port=5432 dbname=nasdb user=postgres sslmode=disable ");
}

TEST(DatabaseConfigTest, MySqlConnectTimeout) {
  cpp_utils::database::MySqlConfig config;
  config.host = "127.0.0.1";
  config.connect_timeout = 10;

  const cpp_utils::database::ConnectionConfig opts(config);
  EXPECT_EQ(opts.conn_string, "host=127.0.0.1 port=3306 db=qtrade user=root password= connect_timeout=10");
}

TEST(DatabaseConfigTest, OracleConnectTimeout) {
  cpp_utils::database::OracleConfig config;
  config.user = "scott";
  config.connect_timeout = 5;

  const cpp_utils::database::ConnectionConfig opts(config);
  EXPECT_EQ(opts.conn_string, "host=127.0.0.1 port=1521 service=ORCL user=scott password= connect_timeout=5");
}

TEST(DatabaseConfigTest, SqliteBusyTimeout) {
  cpp_utils::database::SqliteConfig config;
  config.database_path = "/tmp/test.db";
  config.busy_timeout = 2;

  const cpp_utils::database::ConnectionConfig opts(config);
  EXPECT_EQ(opts.conn_string, "dbname=/tmp/test.db");
  EXPECT_EQ(opts.soci_options.at("sqlite3.busy_timeout"), "2000");
}

TEST(DatabaseConfigTest, ConnectionPoolConfigLeaseTimeout) {
  cpp_utils::database::ConnectionPoolConfig pool_opts;
  pool_opts.pool_size = 8;
  pool_opts.lease_timeout = 1;
  EXPECT_EQ(pool_opts.pool_size, 8U);
  EXPECT_EQ(pool_opts.lease_timeout, 1);
}
