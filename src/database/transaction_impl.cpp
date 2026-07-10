/// @file      transaction_impl.cpp
/// @brief     SOCI 事务封装实现
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#include "src/database/transaction_impl.hpp"

#include "src/database/soci_helper.hpp"

namespace cpputils::database::detail {

Transaction::Transaction(DatabaseError& last_error, DatabaseType database_type)
  : last_error_(last_error), database_type_(database_type) {}

Transaction::Impl::Impl(soci::session& session) : transaction(session) {}

bool Transaction::Begin(soci::session& session) {
  if (impl_ != nullptr) {
    SetDbErrorMessage(last_error_, database_type_, "transaction already active");
    return false;
  }

  try {
    impl_ = std::make_unique<Impl>(session);
    last_error_.Clear();
    return true;
  } catch (const soci::soci_error& ex) {
    SetDbErrorFromSoci(last_error_, database_type_, ex);
    return false;
  }
}

bool Transaction::Commit() {
  if (impl_ == nullptr) {
    SetDbErrorMessage(last_error_, database_type_, "no active transaction");
    return false;
  }
  if (impl_->finished) {
    SetDbErrorMessage(last_error_, database_type_, "transaction already finished");
    return false;
  }

  try {
    impl_->transaction.commit();
    impl_->finished = true;
    impl_.reset();
    last_error_.Clear();
    return true;
  } catch (const soci::soci_error& ex) {
    SetDbErrorFromSoci(last_error_, database_type_, ex);
    return false;
  }
}

bool Transaction::Rollback() {
  if (impl_ == nullptr) {
    last_error_.Clear();
    return true;
  }
  if (impl_->finished) {
    impl_.reset();
    last_error_.Clear();
    return true;
  }

  try {
    impl_->transaction.rollback();
    impl_->finished = true;
    impl_.reset();
    last_error_.Clear();
    return true;
  } catch (const soci::soci_error& ex) {
    SetDbErrorFromSoci(last_error_, database_type_, ex);
    return false;
  }
}

void Transaction::RollbackIfActive() noexcept {
  if (impl_ == nullptr || impl_->finished) {
    impl_.reset();
    return;
  }

  try {
    impl_->transaction.rollback();
  } catch (...) {
  }
  impl_.reset();
}

}  // namespace cpputils::database::detail
