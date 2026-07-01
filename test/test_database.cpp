#include "include/database/database.hpp"

#include <gtest/gtest.h>

namespace {

constexpr const char* kSchemaSql = R"(
CREATE TABLE kv_store (
  config_key TEXT PRIMARY KEY,
  config_value TEXT NOT NULL,
  version INTEGER NOT NULL
);
)";

}  // namespace

TEST(DatabaseConnectionTest, SqliteConnectAndQuery) {
  cpp_utils::database::ConnectionOptions opts;
  opts.database_type = cpp_utils::database::DatabaseType::kSqlite3;
  opts.conn_string = "dbname=:memory:";

  cpp_utils::database::Connection connection(opts);
  ASSERT_EQ(connection.Connect(), cpp_utils::database::Error::kSuccess);

  ASSERT_EQ(connection.Execute(kSchemaSql), cpp_utils::database::Error::kSuccess);
  ASSERT_EQ(connection.Execute("INSERT INTO kv_store(config_key, config_value, version) VALUES('k1', 'v1', 1)"),
            cpp_utils::database::Error::kSuccess);

  auto [query_err, result] = connection.Query("SELECT config_key, config_value, version FROM kv_store");
  ASSERT_EQ(query_err, cpp_utils::database::Error::kSuccess);
  ASSERT_NE(result, nullptr);

  auto row = result->Fetch();
  ASSERT_TRUE(row.has_value());
  EXPECT_EQ(row->get_value("config_key")->as_string().value_or(""), "k1");
  EXPECT_EQ(row->get_value("config_value")->as_string().value_or(""), "v1");
  EXPECT_EQ(row->get_value("version")->as_int64().value_or(0), 1);
  EXPECT_FALSE(result->Fetch().has_value());
}

TEST(DatabaseConnectionTest, TransactionRollback) {
  cpp_utils::database::ConnectionOptions opts;
  opts.database_type = cpp_utils::database::DatabaseType::kSqlite3;
  opts.conn_string = "dbname=:memory:";

  cpp_utils::database::Connection connection(opts);
  ASSERT_EQ(connection.Connect(), cpp_utils::database::Error::kSuccess);
  ASSERT_EQ(connection.Execute(kSchemaSql), cpp_utils::database::Error::kSuccess);
  ASSERT_EQ(connection.Execute("INSERT INTO kv_store(config_key, config_value, version) VALUES('k1', 'v1', 1)"),
            cpp_utils::database::Error::kSuccess);

  auto [begin_err, tx] = connection.BeginTransaction();
  ASSERT_EQ(begin_err, cpp_utils::database::Error::kSuccess);
  ASSERT_EQ(connection.Execute("INSERT INTO kv_store(config_key, config_value, version) VALUES('k2', 'v2', 1)"),
            cpp_utils::database::Error::kSuccess);
  ASSERT_EQ(tx.Rollback(), cpp_utils::database::Error::kSuccess);

  auto [query_err, result] = connection.Query("SELECT config_key FROM kv_store");
  ASSERT_EQ(query_err, cpp_utils::database::Error::kSuccess);
  ASSERT_NE(result, nullptr);

  std::size_t row_count = 0;
  while (result->Fetch().has_value()) {
    ++row_count;
  }
  EXPECT_EQ(row_count, 1U);
}

TEST(DatabaseConnectionTest, NotConnectedReturnsError) {
  cpp_utils::database::Connection connection(cpp_utils::database::ConnectionOptions{});
  auto [query_err, result] = connection.Query("SELECT 1");
  EXPECT_EQ(query_err, cpp_utils::database::Error::kNotConnected);
  EXPECT_EQ(result, nullptr);
}

TEST(DatabaseConnectionPoolTest, AcquireAndQuery) {
  cpp_utils::database::SqliteConfig sqlite_cfg;
  sqlite_cfg.database_path = ":memory:";

  auto pool = cpp_utils::database::CreateConnectionPool();
  ASSERT_EQ(
    pool->Open(cpp_utils::database::ConnectionPoolOptions{cpp_utils::database::ConnectionOptions{sqlite_cfg}, 2}),
    cpp_utils::database::Error::kSuccess);

  auto conn = pool->Acquire();
  ASSERT_NE(conn, nullptr);
  ASSERT_EQ(conn->Execute(kSchemaSql), cpp_utils::database::Error::kSuccess);

  auto [query_err, result] = conn->Query("SELECT 1 AS value");
  ASSERT_EQ(query_err, cpp_utils::database::Error::kSuccess);
  ASSERT_NE(result, nullptr);
  EXPECT_TRUE(result->Fetch().has_value());
}

TEST(DatabaseConnectionTest, StreamingLargeResultUsesConstantMemory) {
  cpp_utils::database::ConnectionOptions opts;
  opts.database_type = cpp_utils::database::DatabaseType::kSqlite3;
  opts.conn_string = "dbname=:memory:";

  cpp_utils::database::Connection connection(opts);
  ASSERT_EQ(connection.Connect(), cpp_utils::database::Error::kSuccess);
  ASSERT_EQ(connection.Execute(kSchemaSql), cpp_utils::database::Error::kSuccess);

  for (int i = 0; i < 100; ++i) {
    const std::string sql =
      "INSERT INTO kv_store(config_key, config_value, version) VALUES('k" + std::to_string(i) + "', 'v', 1)";
    ASSERT_EQ(connection.Execute(sql), cpp_utils::database::Error::kSuccess);
  }

  auto [query_err, result] = connection.Query("SELECT config_key FROM kv_store");
  ASSERT_EQ(query_err, cpp_utils::database::Error::kSuccess);

  std::size_t count = 0;
  while (result->Fetch().has_value()) {
    ++count;
  }
  EXPECT_EQ(count, 100U);
}
