#ifndef CPP_UTILS_DATABASE_DETAIL_TRANSACTION_IMPL_HPP_
#define CPP_UTILS_DATABASE_DETAIL_TRANSACTION_IMPL_HPP_

#include <soci/soci.h>

namespace cpp_utils::database::detail {

/// @brief SOCI 事务 pimpl（供 Transaction 与各连接实现使用）
struct TransactionImpl {
  explicit TransactionImpl(soci::session& session) : transaction(session) {}

  soci::transaction transaction;
  bool finished = false;
};

}  // namespace cpp_utils::database::detail

#endif  // CPP_UTILS_DATABASE_DETAIL_TRANSACTION_IMPL_HPP_
