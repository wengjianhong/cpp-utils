#include "src/database/detail/connection_pool_impl.hpp"

#include <cpputils/database/types.hpp>
#include "src/database/detail/soci_helper.hpp"
#include "src/database/detail/transaction_impl.hpp"

#include <soci/connection-pool.h>
#include <soci/soci.h>

#include <memory>
#include <optional>
#include <utility>

namespace cpp_utils::database {

struct PoolState {
  ConnectionPoolConfig config;
  std::unique_ptr<soci::connection_parameters> params;
  std::unique_ptr<soci::connection_pool> pool;
};

namespace {

class LeasedConnection final : public IConnection {
 public:
  LeasedConnection(std::shared_ptr<PoolState> state, std::size_t pos)
    : state_(std::move(state)), pos_(pos), valid_(true) {}

  ~LeasedConnection() override {
    if (valid_ && state_ && state_->pool) {
      try {
        state_->pool->give_back(pos_);
      } catch (...) {
      }
    }
  }

  bool Connect() override { return IsConnected(); }

  void Disconnect() override { valid_ = false; }

  bool IsConnected() const override { return valid_ && state_ && state_->pool; }

  bool Execute(const std::string& sql, std::int64_t* affected_rows) override {
    if (!IsConnected()) {
      detail::SetDbErrorMessage(last_error_, state_->config.connection.database_type, "leased connection is invalid");
      return false;
    }
    return detail::ExecuteSql(state_->pool->at(pos_), sql, affected_rows, last_error_, state_->config.connection.database_type);
  }

  std::unique_ptr<IResultSet> Query(const std::string& sql) override {
    if (!IsConnected()) {
      detail::SetDbErrorMessage(last_error_, state_->config.connection.database_type, "leased connection is invalid");
      return nullptr;
    }
    return detail::QuerySql(state_->pool->at(pos_), sql, last_error_, state_->config.connection.database_type);
  }

  const DbError& LastError() const override { return last_error_; }

  std::optional<Transaction> BeginTransaction() override {
    if (!IsConnected()) {
      detail::SetDbErrorMessage(last_error_, state_->config.connection.database_type, "leased connection is invalid");
      return std::nullopt;
    }
    try {
      auto tx_impl = std::make_unique<detail::TransactionImpl>(state_->pool->at(pos_));
      return Transaction(this, std::move(tx_impl));
    } catch (const soci::soci_error& ex) {
      detail::SetDbErrorFromSoci(last_error_, state_->config.connection.database_type, ex);
      return std::nullopt;
    }
  }

 protected:
  void SetLastError(DbError error) override {
    if (error.database_type == DatabaseType::kUnknown && state_) {
      error.database_type = state_->config.connection.database_type;
    }
    last_error_ = std::move(error);
  }

 private:
  std::shared_ptr<PoolState> state_;
  std::size_t pos_ = 0;
  bool valid_ = false;
  DbError last_error_;
};

}  // namespace

bool ConnectionPool::Open(const ConnectionPoolConfig& config) {
  if (config.connection.conn_string.empty() || config.pool_size == 0) {
    detail::SetDbErrorMessage(last_error_, config.connection.database_type, "invalid connection pool config");
    return false;
  }

  try {
    auto state = std::make_shared<PoolState>();
    state->config = config;
    state->params = std::make_unique<soci::connection_parameters>(
      std::string(GetDatabaseTypeName(config.connection.database_type)), config.connection.conn_string);
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

bool ConnectionPool::IsOpen() const { return opened_ && state_ != nullptr; }

std::unique_ptr<IConnection> ConnectionPool::Acquire() {
  if (!IsOpen()) {
    return nullptr;
  }

  std::size_t pos = 0;
  const unsigned int lease_timeout_ms =
    state_->config.lease_timeout > 0 ? static_cast<unsigned int>(state_->config.lease_timeout * 1000) : 0;
  if (!state_->pool->try_lease(pos, lease_timeout_ms)) {
    detail::SetDbErrorMessage(last_error_, state_->config.connection.database_type, "no available connection in pool");
    return nullptr;
  }

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

std::unique_ptr<IConnectionPool> CreateConnectionPool() { return std::make_unique<ConnectionPool>(); }

}  // namespace cpp_utils::database
