#ifndef CPP_UTILS_DATABASE_DETAIL_CONNECTION_IMPL_HPP_
#define CPP_UTILS_DATABASE_DETAIL_CONNECTION_IMPL_HPP_

#include <cpputils/database/connection.hpp>

#include <memory>
#include <optional>

namespace cpp_utils::database {

class ConnectionImpl;

/// @brief IConnection 的默认 SOCI 实现（仅库内使用，不对外暴露）
class Connection final : public IConnection {
 public:
  explicit Connection(ConnectionConfig config);
  ~Connection() override;

  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;
  Connection(Connection&& other) noexcept;
  Connection& operator=(Connection&& other) noexcept;

  void Disconnect() override;
  [[nodiscard]] bool Connect() override;
  [[nodiscard]] bool IsConnected() const override;
  [[nodiscard]] std::unique_ptr<IResultSet> Query(const std::string& sql) override;
  [[nodiscard]] bool Execute(const std::string& sql, std::int64_t* affected_rows = nullptr) override;
  [[nodiscard]] const DbError& LastError() const override;
  [[nodiscard]] std::optional<Transaction> BeginTransaction() override;

 protected:
  void SetLastError(DbError error) override;

 private:
  std::unique_ptr<ConnectionImpl> impl_;
};

}  // namespace cpp_utils::database

#endif  // CPP_UTILS_DATABASE_DETAIL_CONNECTION_IMPL_HPP_
