#include <cpputils/database/connection_pool.hpp>

#include <cpputils/database/types.hpp>
#include "src/database/detail/soci_helper.hpp"

#include <soci/connection-pool.h>
#include <soci/soci.h>

#include <memory>
#include <utility>

namespace cpp_utils::database {
namespace {

struct PoolState {
  ConnectionPoolOptions options;
  std::unique_ptr<soci::connection_parameters> params;
  std::unique_ptr<soci::connection_pool> pool;
};

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

  Error Connect() override { return IsConnected() ? Error::kSuccess : Error::kNotConnected; }

  void Disconnect() override { valid_ = false; }

  bool IsConnected() const override { return valid_ && state_ && state_->pool; }

  Error Execute(const std::string& sql, ExecuteResult* out) override {
    if (!IsConnected()) {
      last_error_ = "leased connection is invalid";
      return Error::kNotConnected;
    }
    return detail::ExecuteSql(state_->pool->at(pos_), sql, out, last_error_);
  }

  std::pair<Error, std::unique_ptr<IResultSet>> Query(const std::string& sql) override {
    if (!IsConnected()) {
      last_error_ = "leased connection is invalid";
      return {Error::kNotConnected, nullptr};
    }
    return detail::QuerySql(state_->pool->at(pos_), sql, last_error_);
  }

  const std::string& LastError() const override { return last_error_; }

  std::pair<Error, Transaction> BeginTransaction() override {
    if (!IsConnected()) {
      last_error_ = "leased connection is invalid";
      return {Error::kNotConnected, Transaction(this, nullptr)};
    }
    try {
      auto tx_impl = std::make_unique<detail::TransactionImpl>(state_->pool->at(pos_));
      return {Error::kSuccess, Transaction(this, std::move(tx_impl))};
    } catch (const soci::soci_error& ex) {
      last_error_ = ex.what();
      return {Error::kTransactionFailed, Transaction(this, nullptr)};
    }
  }

 protected:
  void SetLastError(const std::string& message) override { last_error_ = message; }

 private:
  std::shared_ptr<PoolState> state_;
  std::size_t pos_ = 0;
  bool valid_ = false;
  std::string last_error_;
};

class ConnectionPool final : public IConnectionPool {
 public:
  Error Open(const ConnectionPoolOptions& options) override {
    if (options.connection.conn_string.empty() || options.pool_size == 0) {
      last_error_ = "invalid connection pool options";
      return Error::kInvalidArgument;
    }

    try {
      auto state = std::make_shared<PoolState>();
      state->options = options;
      state->params = std::make_unique<soci::connection_parameters>(
        std::string(ToSociBackendName(options.connection.database_type)), options.connection.conn_string);
      for (const auto& [key, value] : options.connection.soci_options) {
        state->params->set_option(key.c_str(), value);
      }
      state->pool = std::make_unique<soci::connection_pool>(options.pool_size);
      for (std::size_t i = 0; i < options.pool_size; ++i) {
        soci::session& session = state->pool->at(i);
        session.open(*state->params);
      }

      state_ = std::move(state);
      opened_ = true;
      last_error_.clear();
      return Error::kSuccess;
    } catch (const soci::soci_error& ex) {
      last_error_ = ex.what();
      state_.reset();
      opened_ = false;
      return Error::kConnectFailed;
    }
  }

  void Close() noexcept override {
    opened_ = false;
    state_.reset();
  }

  bool IsOpen() const override { return opened_ && state_ != nullptr; }

  std::unique_ptr<IConnection> Acquire() override {
    if (!IsOpen()) {
      return nullptr;
    }

    std::size_t pos = 0;
    const unsigned int lease_timeout_ms =
      state_->options.lease_timeout > 0 ? static_cast<unsigned int>(state_->options.lease_timeout * 1000) : 0;
    if (!state_->pool->try_lease(pos, lease_timeout_ms)) {
      last_error_ = "no available connection in pool";
      return nullptr;
    }

    try {
      soci::session& session = state_->pool->at(pos);
      if (!session.is_connected()) {
        session.open(*state_->params);
      }
      return std::make_unique<LeasedConnection>(state_, pos);
    } catch (const soci::soci_error& ex) {
      last_error_ = ex.what();
      state_->pool->give_back(pos);
      return nullptr;
    }
  }

 private:
  std::shared_ptr<PoolState> state_;
  bool opened_ = false;
  std::string last_error_;
};

}  // namespace

std::unique_ptr<IConnectionPool> CreateConnectionPool() { return std::make_unique<ConnectionPool>(); }

}  // namespace cpp_utils::database
