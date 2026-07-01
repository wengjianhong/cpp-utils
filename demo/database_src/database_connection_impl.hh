#ifndef UGREEN_CORE_DATABASE_DATABASE_CONNECTION_IMPL_HH_
#define UGREEN_CORE_DATABASE_DATABASE_CONNECTION_IMPL_HH_

// system headers
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

// third-party headers
#include <soci/connection-pool.h>

// ugreen headers
#include <ugreen/ugos/core/database/database_connection.hh>
#include <ugreen/ugos/core/database/database_connection_pool.hh>
#include <ugreen/ugos/core/database/result_set.hh>

#include "connection_pool.hh"

namespace ugreen::core::database {

/** @brief 数据库连接实现类（RAII 管理）
 * @details 封装单个数据库连接，析构时自动归还连接池
 */
class DatabaseConnectionImpl : public IDatabaseConnection {
 public:
  /// @warning 构造函数文档需明确：pool 必须比此连接实例生命周期更长
  DatabaseConnectionImpl(soci::connection_pool* soci_pool, std::size_t pos, IConnectionPool& pool);
  virtual ~DatabaseConnectionImpl() noexcept override;

  bool IsValid() const override;
  void* GetSessionPtr() override;

  // ==================== SQL 基本方法 ====================
  std::unique_ptr<IResultSet> Query(const std::string& sql_str) override;
  bool Execute(const std::string& sql_str) override;
  int64_t Update(const std::string& sql_str) override;

  // ==================== DML 操作接口 ====================
  bool Insert(const std::string& table_name, const std::map<std::string, std::string>& values) override;
  int64_t Update(const std::string& table_name,
                 const std::map<std::string, std::string>& values,
                 const std::map<std::string, std::string>& where_conditions) override;
  int64_t Delete(const std::string& table_name, const std::map<std::string, std::string>& where_conditions) override;
  std::unique_ptr<IResultSet> Select(const std::string& table_name,
                                     const std::vector<std::string>& columns = {},
                                     const std::map<std::string, std::string>& where_conditions = {},
                                     const std::string& order_by = "",
                                     std::size_t limit = 0) override;
  int64_t Count(const std::string& table_name,
                const std::map<std::string, std::string>& where_conditions = {}) override;

  std::string Escape(const std::string& sql) {
    soci::session& session = soci_pool_->at(pos_);
    return pool_->EscapeIdentifier(sql, &session);
  }

 private:
  /// 执行 SQL 并返回受影响行数（内部函数，用于 Insert/Update/Delete ORM 方法）
  int64_t ExecuteWithParams(const std::string& sql_str, const std::map<std::string, std::string>& params);

  bool valid_;
  std::size_t pos_;
  soci::connection_pool* soci_pool_;
  IConnectionPool* pool_;
};

}  // namespace ugreen::core::database

#endif  // UGREEN_CORE_DATABASE_DATABASE_CONNECTION_IMPL_HH_
