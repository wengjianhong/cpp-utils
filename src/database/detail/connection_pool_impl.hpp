#ifndef CPP_UTILS_DATABASE_DETAIL_CONNECTION_POOL_IMPL_HPP_
#define CPP_UTILS_DATABASE_DETAIL_CONNECTION_POOL_IMPL_HPP_

#include <cpputils/database/connection_pool.hpp>

#include <memory>

namespace cpp_utils::database {

struct PoolState;

/// @brief IConnectionPool 的默认 SOCI 实现（仅库内使用，不对外暴露）
class ConnectionPool final : public IConnectionPool {
 public:
  bool Open(const ConnectionPoolConfig& config) override;
  void Close() noexcept override;
  [[nodiscard]] bool IsOpen() const override;
  [[nodiscard]] std::unique_ptr<IConnection> Acquire() override;

 private:
  std::shared_ptr<PoolState> state_;
  bool opened_ = false;
  DbError last_error_;
};

}  // namespace cpp_utils::database

#endif  // CPP_UTILS_DATABASE_DETAIL_CONNECTION_POOL_IMPL_HPP_
