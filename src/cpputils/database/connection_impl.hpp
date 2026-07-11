/// @file      connection_impl.hpp
/// @brief     IConnection 的 SOCI 实现声明
/// @details   仅库内编译使用，不安装到 include 目录
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_DETAIL_CONNECTION_IMPL_HPP_
#define CPP_UTILS_DATABASE_DETAIL_CONNECTION_IMPL_HPP_

#include <cpputils/database/connection.hpp>

#include <memory>

namespace cpputils::database {

class ConnectionImpl;

/// @brief IConnection 的默认 SOCI 实现（仅库内使用，不对外暴露）
class Connection final : public IConnection {
 public:
  /// @brief 构造连接对象（尚未 Connect）
  /// @param config 连接配置
  explicit Connection(ConnectionConfig config);

  /// @brief 析构时自动 Disconnect
  ~Connection() override;

  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;

  /// @brief 移动构造
  /// @param other 源对象
  Connection(Connection&& other) noexcept;

  /// @brief 移动赋值
  /// @param other 源对象
  /// @return 当前对象引用
  Connection& operator=(Connection&& other) noexcept;

  /// @brief 断开当前连接
  void Disconnect() override;

  /// @brief 建立连接
  /// @return true 表示成功；失败时见 LastError()
  [[nodiscard]] bool Connect() override;

  /// @brief 是否已连接
  /// @return true 表示 session 可用
  [[nodiscard]] bool IsConnected() const override;

  /// @brief 流式查询
  /// @param sql SQL 语句
  /// @return 成功时非空结果集；失败时 nullptr
  [[nodiscard]] std::unique_ptr<IResultSet> Query(const std::string& sql) override;

  /// @brief 执行非查询语句
  /// @param sql SQL 语句
  /// @param affected_rows 可选，写入受影响行数
  /// @return true 表示成功
  [[nodiscard]] bool Execute(const std::string& sql, std::int64_t* affected_rows = nullptr) override;

  /// @brief 最近一次操作的错误详情
  /// @return 错误详情引用
  [[nodiscard]] const DatabaseError& LastError() const override;

  /// @brief 开启事务
  /// @return true 表示成功
  [[nodiscard]] bool BeginTransaction() override;

  /// @brief 提交当前事务
  /// @return true 表示成功
  [[nodiscard]] bool CommitTransaction() override;

  /// @brief 回滚当前事务
  /// @return true 表示成功
  [[nodiscard]] bool RollbackTransaction() override;

 private:
  std::unique_ptr<ConnectionImpl> impl_;  ///< 实现细节（隐藏 SOCI 类型）
};

}  // namespace cpputils::database

#endif  // CPP_UTILS_DATABASE_DETAIL_CONNECTION_IMPL_HPP_
