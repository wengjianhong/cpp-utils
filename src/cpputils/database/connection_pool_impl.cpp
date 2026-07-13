/// @file      connection_pool_impl.cpp
/// @brief     IConnectionPool SOCI 实现
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#include "cpputils/database/connection_pool_impl.hpp"

#include "cpputils/database/soci_helper.hpp"
#include "cpputils/database/transaction_impl.hpp"

#include <cpputils/database/database_types.hpp>

#include <soci/connection-pool.h>
#include <soci/soci.h>

#include <memory>
#include <utility>

namespace cpputils::database {

/// @brief 连接池共享状态（参数、底层 pool）
struct PoolState {
  ConnectionPoolConfig config;                          ///< 池配置
  std::unique_ptr<soci::connection_parameters> params;  ///< SOCI 连接参数
  std::unique_ptr<soci::connection_pool> pool;          ///< SOCI 连接池
};

namespace {

/// @brief 从池中借出的连接包装（析构时自动 give_back）
class LeasedConnection final : public IConnection {
 public:
  /// @brief 构造借出连接
  /// @param state 共享池状态
  /// @param pos 池中槽位下标
  LeasedConnection(std::shared_ptr<PoolState> state, std::size_t pos)
    : state_(std::move(state)),
      pos_(pos),
      valid_(true),
      transaction_(last_error_, state_->config.connection.database_type) {}

  ~LeasedConnection() override {
    transaction_.RollbackIfActive();
    if (valid_ && state_ && state_->pool) {
      try {
        state_->pool->give_back(pos_);
      } catch (...) {
      }
    }
  }

  bool Connect() override {
    return IsConnected();
  }

  bool IsConnected() const override {
    return valid_ && state_ && state_->pool;
  }

  bool Execute(const std::string& sql, std::int64_t* affected_rows) override {
    if (!IsConnected()) {
      detail::SetDbErrorMessage(last_error_, state_->config.connection.database_type, "leased connection is invalid");
      return false;
    }
    return detail::ExecuteSql(
      state_->pool->at(pos_), sql, affected_rows, last_error_, state_->config.connection.database_type);
  }

  std::unique_ptr<IResultSet> Query(const std::string& sql) override {
    if (!IsConnected()) {
      detail::SetDbErrorMessage(last_error_, state_->config.connection.database_type, "leased connection is invalid");
      return nullptr;
    }
    return detail::QuerySql(state_->pool->at(pos_), sql, last_error_, state_->config.connection.database_type);
  }

  const DatabaseError& LastError() const override {
    return last_error_;
  }

  bool BeginTransaction() override {
    if (!IsConnected()) {
      detail::SetDbErrorMessage(last_error_, state_->config.connection.database_type, "leased connection is invalid");
      return false;
    }
    return transaction_.Begin(state_->pool->at(pos_));
  }

  bool CommitTransaction() override {
    return transaction_.Commit();
  }

  bool RollbackTransaction() override {
    return transaction_.Rollback();
  }

  void Disconnect() override {
    transaction_.RollbackIfActive();
    valid_ = false;
  }

 private:
  std::shared_ptr<PoolState> state_;  ///< 共享池状态
  std::size_t pos_ = 0;               ///< 池中槽位
  bool valid_ = false;                ///< 是否仍可操作
  DatabaseError last_error_;          ///< 最近一次操作错误
  detail::Transaction transaction_;   ///< 事务状态
};

}  // namespace

bool ConnectionPool::Open(const ConnectionPoolConfig& config) {
  // 1. 校验配置
  if (config.connection.conn_string.empty() || config.pool_size == 0) {
    detail::SetDbErrorMessage(last_error_, config.connection.database_type, "invalid connection pool config");
    return false;
  }

  // 2. 创建 SOCI 连接池并预打开所有 session
  try {
    auto state = std::make_shared<PoolState>();
    state->config = config;
    state->params = std::make_unique<soci::connection_parameters>(
      std::string(GetDatabaseNameByType(config.connection.database_type)), config.connection.conn_string);
    for (const auto& [key, value] : config.connection.soci_options) {
      state->params->set_option(key.c_str(), value);
    }
    state->pool = std::make_unique<soci::connection_pool>(config.pool_size);
    for (std::size_t i = 0; i < config.pool_size; ++i) {
      soci::session& session = state->pool->at(i);
      session.open(*state->params);
    }

    state_ = std::move(state);
    opened_ = true;
    last_error_.Clear();
    return true;
  } catch (const soci::soci_error& ex) {
    detail::SetDbErrorFromSoci(last_error_, config.connection.database_type, ex);
    state_.reset();
    opened_ = false;
    return false;
  }
}

void ConnectionPool::Close() noexcept {
  opened_ = false;
  state_.reset();
}

bool ConnectionPool::IsOpen() const {
  return opened_ && state_ != nullptr;
}

std::unique_ptr<IConnection> ConnectionPool::Acquire() {
  if (!IsOpen()) {
    return nullptr;
  }

  // 1. 尝试借出槽位（带超时）
  std::size_t pos = 0;
  const unsigned int lease_timeout_ms =
    state_->config.lease_timeout > 0 ? static_cast<unsigned int>(state_->config.lease_timeout * 1000) : 0;
  if (!state_->pool->try_lease(pos, lease_timeout_ms)) {
    detail::SetDbErrorMessage(last_error_, state_->config.connection.database_type, "no available connection in pool");
    return nullptr;
  }

  // 2. 确保 session 已连接并返回 LeasedConnection
  try {
    soci::session& session = state_->pool->at(pos);
    if (!session.is_connected()) {
      session.open(*state_->params);
    }
    return std::make_unique<LeasedConnection>(state_, pos);
  } catch (const soci::soci_error& ex) {
    detail::SetDbErrorFromSoci(last_error_, state_->config.connection.database_type, ex);
    state_->pool->give_back(pos);
    return nullptr;
  }
}

std::unique_ptr<IConnectionPool> CreateConnectionPool() {
  return std::make_unique<ConnectionPool>();
}

}  // namespace cpputils::database
