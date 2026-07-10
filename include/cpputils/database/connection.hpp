/// @file      connection.hpp
/// @brief     数据库连接抽象
/// @details   头文件不暴露 SOCI 类型；具体实现见 CreateConnection()
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_CONNECTION_HPP_
#define CPP_UTILS_DATABASE_CONNECTION_HPP_

#include <cpputils/database/config.hpp>
#include <cpputils/database/database_error.hpp>
#include <cpputils/database/result_set.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace cpputils::database {

/// @brief 数据库连接抽象（便于 mock / 连接池借还）
class IConnection {
 public:
  virtual ~IConnection() = default;

  /// @brief 断开当前连接
  virtual void Disconnect() = 0;

  /// @brief 建立连接
  /// @return true 表示成功；失败时见 LastError()
  [[nodiscard]] virtual bool Connect() = 0;

  /// @brief 是否已连接
  /// @return true 表示可用
  [[nodiscard]] virtual bool IsConnected() const = 0;

  /// @brief 最近一次操作的错误详情
  /// @return 错误详情引用；无错误时 Ok() 为 true
  [[nodiscard]] virtual const DatabaseError& LastError() const = 0;

  /// @brief 流式查询，返回结果集供逐行 Fetch
  /// @param sql SQL 语句
  /// @return 成功时非空结果集；失败时 nullptr，见 LastError()
  [[nodiscard]] virtual std::unique_ptr<IResultSet> Query(const std::string& sql) = 0;

  /// @brief 执行非查询语句（INSERT/UPDATE/DELETE 等）
  /// @param sql SQL 语句
  /// @param affected_rows 可选，写入受影响行数（SOCI 约定：-1 表示未知）
  /// @return true 表示成功；失败时见 LastError()
  [[nodiscard]] virtual bool Execute(const std::string& sql, std::int64_t* affected_rows = nullptr) = 0;

  /// @brief 开启事务（同一连接同时仅允许一个活跃事务）
  /// @return true 表示成功；失败时见 LastError()
  [[nodiscard]] virtual bool BeginTransaction() = 0;

  /// @brief 提交当前事务
  /// @return true 表示成功；无活跃事务或失败时见 LastError()
  [[nodiscard]] virtual bool CommitTransaction() = 0;

  /// @brief 回滚当前事务
  /// @return true 表示成功；无活跃事务时返回 true
  [[nodiscard]] virtual bool RollbackTransaction() = 0;
};

/// @brief 创建默认 SOCI 单连接实现
/// @param config 连接配置
/// @return 未 Connect 的连接对象
[[nodiscard]] std::unique_ptr<IConnection> CreateConnection(ConnectionConfig config);

}  // namespace cpputils::database

#endif  // CPP_UTILS_DATABASE_CONNECTION_HPP_
