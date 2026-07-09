#include "src/database/detail/connection_impl.hpp"

#include <cpputils/database/types.hpp>
#include "src/database/detail/soci_helper.hpp"
#include "src/database/detail/transaction_impl.hpp"

#include <soci/soci.h>

#include <utility>

namespace cpp_utils::database {

class ConnectionImpl {
 public:
  explicit ConnectionImpl(ConnectionConfig config) : config_(std::move(config)) {}

  ConnectionConfig config_;
  std::unique_ptr<soci::session> session_;
  DbError last_error_;
};

Connection::Connection(ConnectionConfig config) : impl_(std::make_unique<ConnectionImpl>(std::move(config))) {}

Connection::~Connection() { Disconnect(); }

Connection::Connection(Connection&& other) noexcept = default;

Connection& Connection::operator=(Connection&& other) noexcept = default;

void Connection::SetLastError(DbError error) {
  if (error.database_type == DatabaseType::kUnknown) {
    error.database_type = impl_->config_.database_type;
  }
  impl_->last_error_ = std::move(error);
}

bool Connection::Connect() {
  if (impl_->config_.conn_string.empty()) {
    detail::SetDbErrorMessage(impl_->last_error_, impl_->config_.database_type, "connection string is empty");
    return false;
  }

  Disconnect();

  try {
    impl_->session_ = std::make_unique<soci::session>(std::string(GetDatabaseTypeName(impl_->config_.database_type)),
                                                      impl_->config_.conn_string);
    impl_->last_error_.Clear();
    return true;
  } catch (const soci::soci_error& ex) {
    detail::SetDbErrorFromSoci(impl_->last_error_, impl_->config_.database_type, ex);
    impl_->session_.reset();
    return false;
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

std::unique_ptr<IResultSet> Connection::Query(const std::string& sql) {
  if (!IsConnected()) {
    detail::SetDbErrorMessage(impl_->last_error_, impl_->config_.database_type, "session not connected");
    return nullptr;
  }
  return detail::QuerySql(*impl_->session_, sql, impl_->last_error_, impl_->config_.database_type);
}

bool Connection::Execute(const std::string& sql, std::int64_t* affected_rows) {
  if (!IsConnected()) {
    detail::SetDbErrorMessage(impl_->last_error_, impl_->config_.database_type, "session not connected");
    return false;
  }
  return detail::ExecuteSql(*impl_->session_, sql, affected_rows, impl_->last_error_, impl_->config_.database_type);
}

const DbError& Connection::LastError() const { return impl_->last_error_; }

std::optional<Transaction> Connection::BeginTransaction() {
  if (!IsConnected()) {
    detail::SetDbErrorMessage(impl_->last_error_, impl_->config_.database_type, "session not connected");
    return std::nullopt;
  }

  try {
    auto tx_impl = std::make_unique<detail::TransactionImpl>(*impl_->session_);
    return Transaction(this, std::move(tx_impl));
  } catch (const soci::soci_error& ex) {
    detail::SetDbErrorFromSoci(impl_->last_error_, impl_->config_.database_type, ex);
    return std::nullopt;
  }
}

std::unique_ptr<IConnection> CreateConnection(ConnectionConfig config) {
  return std::make_unique<Connection>(std::move(config));
}

}  // namespace cpp_utils::database
