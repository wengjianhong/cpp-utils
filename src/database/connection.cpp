#include "include/database/connection.hpp"

#include "include/database/types.hpp"
#include "src/database/detail/soci_helper.hpp"

#include <soci/soci.h>

#include <utility>

namespace cpp_utils::database {

class ConnectionImpl {
 public:
  explicit ConnectionImpl(ConnectionOptions options) : options_(std::move(options)) {}

  ConnectionOptions options_;
  std::unique_ptr<soci::session> session_;
  std::string last_error_;
};

Connection::Connection(ConnectionOptions options) : impl_(std::make_unique<ConnectionImpl>(std::move(options))) {}

Connection::~Connection() { Disconnect(); }

Connection::Connection(Connection&& other) noexcept = default;

Connection& Connection::operator=(Connection&& other) noexcept = default;

void Connection::SetLastError(const std::string& message) { impl_->last_error_ = message; }

Error Connection::Connect() {
  if (impl_->options_.conn_string.empty()) {
    impl_->last_error_ = "connection string is empty";
    return Error::kInvalidArgument;
  }

  Disconnect();

  try {
    impl_->session_ = std::make_unique<soci::session>(std::string(ToSociBackendName(impl_->options_.database_type)),
                                                     impl_->options_.conn_string);
    impl_->last_error_.clear();
    return Error::kSuccess;
  } catch (const soci::soci_error& ex) {
    impl_->last_error_ = ex.what();
    impl_->session_.reset();
    return Error::kConnectFailed;
  }
}

void Connection::Disconnect() {
  if (impl_->session_) {
    try {
      impl_->session_->close();
    } catch (...) {
    }
    impl_->session_.reset();
  }
}

bool Connection::IsConnected() const { return impl_->session_ != nullptr; }

std::pair<Error, std::unique_ptr<IResultSet>> Connection::Query(const std::string& sql) {
  if (!IsConnected()) {
    impl_->last_error_ = "session not connected";
    return {Error::kNotConnected, nullptr};
  }
  return detail::QuerySql(*impl_->session_, sql, impl_->last_error_);
}

Error Connection::Execute(const std::string& sql, ExecuteResult* out) {
  if (!IsConnected()) {
    impl_->last_error_ = "session not connected";
    return Error::kNotConnected;
  }
  return detail::ExecuteSql(*impl_->session_, sql, out, impl_->last_error_);
}

const std::string& Connection::LastError() const { return impl_->last_error_; }

IConnection::Transaction::Transaction(IConnection* connection, std::unique_ptr<detail::TransactionImpl> impl)
    : connection_(connection), impl_(std::move(impl)) {}

IConnection::Transaction::Transaction(Transaction&& other) noexcept = default;

IConnection::Transaction& IConnection::Transaction::operator=(Transaction&& other) noexcept = default;

IConnection::Transaction::~Transaction() {
  if (impl_ != nullptr && !impl_->finished) {
    (void)Rollback();
  }
}

Error IConnection::Transaction::Commit() {
  if (impl_ == nullptr || impl_->finished) {
    return Error::kTransactionFailed;
  }
  try {
    impl_->transaction.commit();
    impl_->finished = true;
    return Error::kSuccess;
  } catch (const soci::soci_error& ex) {
    if (connection_ != nullptr) {
      connection_->SetLastError(ex.what());
    }
    return Error::kTransactionFailed;
  }
}

Error IConnection::Transaction::Rollback() {
  if (impl_ == nullptr || impl_->finished) {
    return Error::kSuccess;
  }
  try {
    impl_->transaction.rollback();
    impl_->finished = true;
    return Error::kSuccess;
  } catch (const soci::soci_error& ex) {
    if (connection_ != nullptr) {
      connection_->SetLastError(ex.what());
    }
    return Error::kTransactionFailed;
  }
}

std::pair<Error, IConnection::Transaction> Connection::BeginTransaction() {
  if (!IsConnected()) {
    impl_->last_error_ = "session not connected";
    return {Error::kNotConnected, Transaction(this, nullptr)};
  }

  try {
    auto tx_impl = std::make_unique<detail::TransactionImpl>(*impl_->session_);
    return {Error::kSuccess, Transaction(this, std::move(tx_impl))};
  } catch (const soci::soci_error& ex) {
    impl_->last_error_ = ex.what();
    return {Error::kTransactionFailed, Transaction(this, nullptr)};
  }
}

}  // namespace cpp_utils::database
