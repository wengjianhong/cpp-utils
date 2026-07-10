#ifndef CPP_UTILS_DATABASE_DETAIL_TRANSACTION_IMPL_HPP_
#define CPP_UTILS_DATABASE_DETAIL_TRANSACTION_IMPL_HPP_

#include <cpputils/database/database_error.hpp>
#include <cpputils/database/database_types.hpp>

#include <soci/soci.h>

#include <memory>

namespace cpputils::database::detail {

/// @brief 单连接上的 SOCI 事务状态（同时仅允许一个活跃事务）
class Transaction {
 public:
  Transaction(DatabaseError& last_error, DatabaseType database_type);

  Transaction(const Transaction&) = delete;
  Transaction& operator=(const Transaction&) = delete;

  /// @brief 在 session 上开启事务
  [[nodiscard]] bool Begin(soci::session& session);

  /// @brief 提交当前事务
  [[nodiscard]] bool Commit();

  /// @brief 回滚当前事务
  [[nodiscard]] bool Rollback();

  /// @brief 若仍有未结束事务则静默回滚（用于析构/断开连接）
  void RollbackIfActive() noexcept;

 private:
  struct Impl {
    explicit Impl(soci::session& session) : transaction(session) {}

    soci::transaction transaction;
    bool finished = false;
  };

  DatabaseError& last_error_;
  DatabaseType database_type_;
  std::unique_ptr<Impl> impl_;
};

}  // namespace cpputils::database::detail

#endif  // CPP_UTILS_DATABASE_DETAIL_TRANSACTION_IMPL_HPP_
