/// @file      transaction.hpp
/// @brief     数据库事务 RAII 句柄
/// @author    wengjianhong
/// @date      2026-07-09
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_TRANSACTION_HPP_
#define CPP_UTILS_DATABASE_TRANSACTION_HPP_

#include <memory>

namespace cpp_utils::database {

class IConnection;

namespace detail {
struct TransactionImpl;
}

/// @brief RAII 事务句柄，析构时未 Commit 则 Rollback
class Transaction {
 public:
  /// @brief 移动构造
  /// @param other 源事务
  Transaction(Transaction&& other) noexcept;

  /// @brief 移动赋值
  /// @param other 源事务
  /// @return 当前对象引用
  Transaction& operator=(Transaction&& other) noexcept;

  /// @brief 析构：若仍持有 impl 则 Rollback
  ~Transaction();

  Transaction(const Transaction&) = delete;
  Transaction& operator=(const Transaction&) = delete;

  /// @brief 提交事务
  /// @return true 表示成功；失败时见所属连接的 LastError()
  [[nodiscard]] bool Commit();

  /// @brief 回滚事务
  /// @return true 表示成功；失败时见所属连接的 LastError()
  [[nodiscard]] bool Rollback();

  /// @brief 内部构造，由 IConnection::BeginTransaction 使用
  /// @param connection 所属连接
  /// @param impl 事务实现
  Transaction(IConnection* connection, std::unique_ptr<detail::TransactionImpl> impl);

 private:
  IConnection* connection_ = nullptr;              ///< 所属连接，可为 nullptr 表示已移交
  std::unique_ptr<detail::TransactionImpl> impl_;  ///< 事务 pimpl
};

}  // namespace cpp_utils::database

#endif  // CPP_UTILS_DATABASE_TRANSACTION_HPP_
