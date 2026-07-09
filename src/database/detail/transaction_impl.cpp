#include <cpputils/database/connection.hpp>
#include <cpputils/database/transaction.hpp>

#include "src/database/detail/transaction_impl.hpp"

#include <soci/soci.h>

#include <utility>

namespace cpp_utils::database {

Transaction::Transaction(IConnection* connection, std::unique_ptr<detail::TransactionImpl> impl)
  : connection_(connection), impl_(std::move(impl)) {}

Transaction::Transaction(Transaction&& other) noexcept = default;

Transaction& Transaction::operator=(Transaction&& other) noexcept = default;

Transaction::~Transaction() {
  if (impl_ != nullptr && !impl_->finished) {
    (void)Rollback();
  }
}

bool Transaction::Commit() {
  if (impl_ == nullptr || impl_->finished) {
    return false;
  }
  try {
    impl_->transaction.commit();
    impl_->finished = true;
    return true;
  } catch (const soci::soci_error& ex) {
    if (connection_ != nullptr) {
      DbError err;
      err.message = ex.what();
      connection_->SetLastError(std::move(err));
    }
    return false;
  }
}

bool Transaction::Rollback() {
  if (impl_ == nullptr || impl_->finished) {
    return true;
  }
  try {
    impl_->transaction.rollback();
    impl_->finished = true;
    return true;
  } catch (const soci::soci_error& ex) {
    if (connection_ != nullptr) {
      DbError err;
      err.message = ex.what();
      connection_->SetLastError(std::move(err));
    }
    return false;
  }
}

}  // namespace cpp_utils::database
