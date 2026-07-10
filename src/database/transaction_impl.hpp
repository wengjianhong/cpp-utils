/// @file      transaction_impl.hpp
/// @brief     单连接 SOCI 事务封装
/// @details   同时仅允许一个活跃事务；析构/断开时自动 RollbackIfActive
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
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
  /// @brief 构造事务管理器
  /// @param last_error 关联连接的错误对象（非拥有引用）
  /// @param database_type 数据库类型（写入错误详情）
  Transaction(DatabaseError& last_error, DatabaseType database_type);

  Transaction(const Transaction&) = delete;
  Transaction& operator=(const Transaction&) = delete;

  /// @brief 在 session 上开启事务
  /// @param session SOCI 会话
  /// @return true 表示成功；已有活跃事务时 false
  [[nodiscard]] bool Begin(soci::session& session);

  /// @brief 提交当前事务
  /// @return true 表示成功；无活跃事务时 false
  [[nodiscard]] bool Commit();

  /// @brief 回滚当前事务
  /// @return true 表示成功；无活跃事务时返回 true
  [[nodiscard]] bool Rollback();

  /// @brief 若仍有未结束事务则静默回滚（用于析构/断开连接）
  void RollbackIfActive() noexcept;

 private:
  /// @brief SOCI transaction 包装
  struct Impl {
    /// @brief 在指定 session 上创建事务
    /// @param session SOCI 会话
    explicit Impl(soci::session& session);

    soci::transaction transaction;  ///< SOCI 事务对象
    bool finished = false;          ///< 是否已 commit/rollback
  };

  DatabaseError& last_error_;   ///< 关联连接的错误对象
  DatabaseType database_type_;  ///< 数据库类型
  std::unique_ptr<Impl> impl_;  ///< 活跃事务；无事务时为 nullptr
};

}  // namespace cpputils::database::detail

#endif  // CPP_UTILS_DATABASE_DETAIL_TRANSACTION_IMPL_HPP_
