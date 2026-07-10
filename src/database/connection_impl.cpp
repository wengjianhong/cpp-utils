#include "src/database/connection_impl.hpp"

#include "src/database/soci_helper.hpp"
#include "src/database/transaction_impl.hpp"
#include <cpputils/database/database_types.hpp>

#include <soci/soci.h>

#include <utility>

namespace cpputils::database {

class ConnectionImpl {
 public:
  explicit ConnectionImpl(ConnectionConfig config)
    : config_(std::move(config)), transaction_(last_error_, config_.database_type) {}

  ConnectionConfig config_;
  std::unique_ptr<soci::session> session_;
  DatabaseError last_error_;
  detail::Transaction transaction_;
};

Connection::Connection(ConnectionConfig config) : impl_(std::make_unique<ConnectionImpl>(std::move(config))) {}

Connection::~Connection() {
  Disconnect();
}

Connection::Connection(Connection&& other) noexcept = default;

Connection& Connection::operator=(Connection&& other) noexcept = default;

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
  impl_->transaction_.RollbackIfActive();
  if (impl_->session_) {
    try {
      impl_->session_->close();
    } catch (...) {
    }
    impl_->session_.reset();
  }
}

bool Connection::IsConnected() const {
  return impl_->session_ != nullptr;
}

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

const DatabaseError& Connection::LastError() const {
  return impl_->last_error_;
}

bool Connection::BeginTransaction() {
  if (!IsConnected()) {
    detail::SetDbErrorMessage(impl_->last_error_, impl_->config_.database_type, "session not connected");
    return false;
  }
  return impl_->transaction_.Begin(*impl_->session_);
}

bool Connection::CommitTransaction() {
  return impl_->transaction_.Commit();
}

bool Connection::RollbackTransaction() {
  return impl_->transaction_.Rollback();
}

std::unique_ptr<IConnection> CreateConnection(ConnectionConfig config) {
  return std::make_unique<Connection>(std::move(config));
}

}  // namespace cpputils::database
