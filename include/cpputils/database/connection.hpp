/// @file      connection.hpp
/// @brief     数据库连接抽象与 RAII 实现
/// @details   头文件不暴露 SOCI 类型，便于 mock 与连接池借还
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef CPP_UTILS_DATABASE_CONNECTION_HPP_
#define CPP_UTILS_DATABASE_CONNECTION_HPP_

#include <cpputils/database/config.hpp>
#include <cpputils/database/error.hpp>
#include <cpputils/database/result_set.hpp>

#include <memory>
#include <string>
#include <utility>

namespace cpp_utils::database::detail {
struct TransactionImpl;
}

namespace cpp_utils::database {

class ConnectionImpl;

/// @brief 数据库连接抽象（便于 mock / 连接池借还）
class IConnection {
 public:
  virtual ~IConnection() = default;

  /// @brief 断开当前连接
  virtual void Disconnect() = 0;

  /// @brief 建立连接
  /// @return kSuccess 表示成功
  [[nodiscard]] virtual Error Connect() = 0;

  /// @brief 是否已连接
  /// @return true 表示可用
  [[nodiscard]] virtual bool IsConnected() const = 0;

  /// @brief 最近一次错误的描述文本
  /// @return 错误消息引用
  [[nodiscard]] virtual const std::string& LastError() const = 0;

  /// @brief 流式查询，返回结果集供逐行 Fetch
  /// @param sql SQL 语句
  /// @return pair<Error, IResultSet>；失败时 second 为 nullptr
  [[nodiscard]] virtual std::pair<Error, std::unique_ptr<IResultSet>> Query(const std::string& sql) = 0;

  /// @brief 执行非查询语句（INSERT/UPDATE/DELETE 等）
  /// @param sql SQL 语句
  /// @param out 可选，写入受影响行数
  /// @return 操作结果码
  [[nodiscard]] virtual Error Execute(const std::string& sql, ExecuteResult* out = nullptr) = 0;

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
    /// @return kSuccess 表示成功
    [[nodiscard]] Error Commit();

    /// @brief 回滚事务
    /// @return kSuccess 表示成功
    [[nodiscard]] Error Rollback();

    /// @brief 内部构造，由 BeginTransaction 使用
    /// @param connection 所属连接
    /// @param impl 事务实现
    Transaction(IConnection* connection, std::unique_ptr<detail::TransactionImpl> impl);

   private:
    IConnection* connection_ = nullptr;              ///< 所属连接，可为 nullptr 表示已移交
    std::unique_ptr<detail::TransactionImpl> impl_;  ///< 事务 pimpl
  };

  /// @brief 开启事务
  /// @return pair<Error, Transaction>；失败时 Transaction 为空壳
  [[nodiscard]] virtual std::pair<Error, Transaction> BeginTransaction() = 0;

 protected:
  /// @brief 设置最近一次错误消息
  /// @param message 错误描述
  virtual void SetLastError(const std::string& message) = 0;
};

/// @brief 单连接 RAII 封装（pimpl，头文件不暴露 SOCI）
class Connection final : public IConnection {
 public:
  /// @brief 构造连接对象（未 Connect）
  /// @param options 连接参数
  explicit Connection(ConnectionOptions options);

  /// @brief 析构并 Disconnect
  ~Connection() override;

  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;

  /// @brief 移动构造
  /// @param other 源连接
  Connection(Connection&& other) noexcept;

  /// @brief 移动赋值
  /// @param other 源连接
  /// @return 当前对象引用
  Connection& operator=(Connection&& other) noexcept;

  /// @brief 断开连接
  void Disconnect() override;

  /// @brief 建立连接
  /// @return kSuccess 表示成功
  [[nodiscard]] Error Connect() override;

  /// @brief 是否已连接
  /// @return true 表示可用
  [[nodiscard]] bool IsConnected() const override;

  /// @brief 流式查询
  /// @param sql SQL 语句
  /// @return pair<Error, IResultSet>
  [[nodiscard]] std::pair<Error, std::unique_ptr<IResultSet>> Query(const std::string& sql) override;

  /// @brief 执行非查询语句
  /// @param sql SQL 语句
  /// @param out 可选受影响行数
  /// @return 操作结果码
  [[nodiscard]] Error Execute(const std::string& sql, ExecuteResult* out = nullptr) override;

  /// @brief 最近一次错误描述
  /// @return 错误消息引用
  [[nodiscard]] const std::string& LastError() const override;

  /// @brief 开启事务
  /// @return pair<Error, Transaction>
  [[nodiscard]] std::pair<Error, Transaction> BeginTransaction() override;

 protected:
  /// @brief 设置最近一次错误消息
  /// @param message 错误描述
  void SetLastError(const std::string& message) override;

 private:
  std::unique_ptr<ConnectionImpl> impl_;  ///< 连接 pimpl
};

}  // namespace cpp_utils::database

#endif  // CPP_UTILS_DATABASE_CONNECTION_HPP_
