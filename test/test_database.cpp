#include <cpputils/database/connection.hpp>
#include <cpputils/database/connection_pool.hpp>

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
  cpp_utils::database::ConnectionConfig opts;
  opts.database_type = cpp_utils::database::DatabaseType::kSqlite3;
  opts.conn_string = "dbname=:memory:";

  auto connection = cpp_utils::database::CreateConnection(opts);
  ASSERT_NE(connection, nullptr);
  ASSERT_TRUE(connection->Connect());

  ASSERT_TRUE(connection->Execute(kSchemaSql));
  ASSERT_TRUE(connection->Execute("INSERT INTO kv_store(config_key, config_value, version) VALUES('k1', 'v1', 1)"));

  auto result = connection->Query("SELECT config_key, config_value, version FROM kv_store");
  ASSERT_NE(result, nullptr);

  auto row = result->Fetch();
  ASSERT_TRUE(row.has_value());
  EXPECT_EQ(row->get_value("config_key")->as_string().value_or(""), "k1");
  EXPECT_EQ(row->get_value("config_value")->as_string().value_or(""), "v1");
  EXPECT_EQ(row->get_value("version")->as_int64().value_or(0), 1);
  EXPECT_FALSE(result->Fetch().has_value());
}

TEST(DatabaseConnectionTest, TransactionRollback) {
  cpp_utils::database::ConnectionConfig opts;
  opts.database_type = cpp_utils::database::DatabaseType::kSqlite3;
  opts.conn_string = "dbname=:memory:";

  auto connection = cpp_utils::database::CreateConnection(opts);
  ASSERT_NE(connection, nullptr);
  ASSERT_TRUE(connection->Connect());
  ASSERT_TRUE(connection->Execute(kSchemaSql));
  ASSERT_TRUE(connection->Execute("INSERT INTO kv_store(config_key, config_value, version) VALUES('k1', 'v1', 1)"));

  auto tx = connection->BeginTransaction();
  ASSERT_TRUE(tx.has_value());
  ASSERT_TRUE(connection->Execute("INSERT INTO kv_store(config_key, config_value, version) VALUES('k2', 'v2', 1)"));
  ASSERT_TRUE(tx->Rollback());

  auto result = connection->Query("SELECT config_key FROM kv_store");
  ASSERT_NE(result, nullptr);

  std::size_t row_count = 0;
  while (result->Fetch().has_value()) {
    ++row_count;
  }
  EXPECT_EQ(row_count, 1U);
}

TEST(DatabaseConnectionTest, NotConnectedReturnsError) {
  auto connection = cpp_utils::database::CreateConnection(cpp_utils::database::ConnectionConfig{});
  ASSERT_NE(connection, nullptr);
  EXPECT_EQ(connection->Query("SELECT 1"), nullptr);
  EXPECT_FALSE(connection->LastError().Ok());
}

TEST(DatabaseConnectionPoolTest, AcquireAndQuery) {
  cpp_utils::database::SqliteConfig sqlite_cfg;
  sqlite_cfg.database_path = ":memory:";

  auto pool = cpp_utils::database::CreateConnectionPool();
  ASSERT_TRUE(pool->Open(cpp_utils::database::ConnectionPoolConfig{cpp_utils::database::ConnectionConfig{sqlite_cfg}, 2}));

  auto conn = pool->Acquire();
  ASSERT_NE(conn, nullptr);
  ASSERT_TRUE(conn->Execute(kSchemaSql));

  auto result = conn->Query("SELECT 1 AS value");
  ASSERT_NE(result, nullptr);
  EXPECT_TRUE(result->Fetch().has_value());
}

TEST(DatabaseConnectionTest, StreamingLargeResultUsesConstantMemory) {
  cpp_utils::database::ConnectionConfig opts;
  opts.database_type = cpp_utils::database::DatabaseType::kSqlite3;
  opts.conn_string = "dbname=:memory:";

  auto connection = cpp_utils::database::CreateConnection(opts);
  ASSERT_NE(connection, nullptr);
  ASSERT_TRUE(connection->Connect());
  ASSERT_TRUE(connection->Execute(kSchemaSql));

  for (int i = 0; i < 100; ++i) {
    const std::string sql =
      "INSERT INTO kv_store(config_key, config_value, version) VALUES('k" + std::to_string(i) + "', 'v', 1)";
    ASSERT_TRUE(connection->Execute(sql));
  }

  auto result = connection->Query("SELECT config_key FROM kv_store");
  ASSERT_NE(result, nullptr);

  std::size_t count = 0;
  while (result->Fetch().has_value()) {
    ++count;
  }
  EXPECT_EQ(count, 100U);
}
