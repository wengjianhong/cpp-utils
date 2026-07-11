/// @file      connection_pool_impl.hpp
/// @brief     IConnectionPool 的 SOCI 实现声明
/// @details   仅库内编译使用，不安装到 include 目录
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_DETAIL_CONNECTION_POOL_IMPL_HPP_
#define CPP_UTILS_DATABASE_DETAIL_CONNECTION_POOL_IMPL_HPP_

#include <cpputils/database/connection_pool.hpp>

#include <memory>

namespace cpputils::database {

struct PoolState;

/// @brief IConnectionPool 的默认 SOCI 实现（仅库内使用，不对外暴露）
class ConnectionPool final : public IConnectionPool {
 public:
  /// @brief 按配置打开连接池
  /// @param config 连接池配置
  /// @return true 表示成功；失败时见 last_error_
  bool Open(const ConnectionPoolConfig& config) override;

  /// @brief 关闭连接池并释放所有连接
  void Close() noexcept override;

  /// @brief 连接池是否已打开
  /// @return true 表示可 Acquire
  [[nodiscard]] bool IsOpen() const override;

  /// @brief 借出连接
  /// @return 可用连接；池未打开或耗尽时返回 nullptr
  [[nodiscard]] std::unique_ptr<IConnection> Acquire() override;

 private:
  std::shared_ptr<PoolState> state_;  ///< 共享池状态（供 LeasedConnection 归还）
  bool opened_ = false;               ///< 是否已成功 Open
  DatabaseError last_error_;          ///< Open/Acquire 失败时的错误详情
};

}  // namespace cpputils::database

#endif  // CPP_UTILS_DATABASE_DETAIL_CONNECTION_POOL_IMPL_HPP_
