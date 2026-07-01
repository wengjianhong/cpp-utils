/// @file      connection_pool.hpp
/// @brief     数据库连接池抽象与工厂
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_CONNECTION_POOL_HPP_
#define CPP_UTILS_DATABASE_CONNECTION_POOL_HPP_

#include <cpputils/database/config.hpp>
#include <cpputils/database/connection.hpp>
#include <cpputils/database/error.hpp>

#include <memory>

namespace cpp_utils::database {

/// @brief 连接池抽象
class IConnectionPool {
 public:
  virtual ~IConnectionPool() = default;

  /// @brief 关闭连接池并释放所有连接
  virtual void Close() noexcept = 0;

  /// @brief 连接池是否已打开
  /// @return true 表示可 Acquire
  [[nodiscard]] virtual bool IsOpen() const = 0;

  /// @brief 按配置打开连接池
  /// @param options 连接池参数（connection + pool_size）
  /// @return kSuccess 表示成功
  [[nodiscard]] virtual Error Open(const ConnectionPoolOptions& options) = 0;

  /// @brief 借出连接；unique_ptr 析构时自动归还
  /// @return 可用连接；池未打开或耗尽时返回 nullptr
  [[nodiscard]] virtual std::unique_ptr<IConnection> Acquire() = 0;
};

/// @brief 创建默认 SOCI 连接池实现
/// @return 连接池 unique_ptr
[[nodiscard]] std::unique_ptr<IConnectionPool> CreateConnectionPool();

}  // namespace cpp_utils::database

#endif  // CPP_UTILS_DATABASE_CONNECTION_POOL_HPP_
