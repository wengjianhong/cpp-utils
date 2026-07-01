#ifndef UGREEN_CORE_DATABASE_DATABASE_CONNECTION_HH_
#define UGREEN_CORE_DATABASE_DATABASE_CONNECTION_HH_

/// @file database_connection.hh
/// @author UGreen NAS Team
/// @brief 数据库连接接口定义
/// @date 2026-01-20
/// @copyright Copyright (c) 2026
/// @details 封装单个数据库连接，析构时自动归还连接池

#include <map>
#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <string_view>

#include <ugreen/ugos/core/database/result_set.hh>

namespace ugreen::core::database {

/// @brief 数据库连接接口（RAII 管理）
/// @details 封装单个数据库连接，析构时自动归还连接池
class IDatabaseConnection {
 public:
  virtual ~IDatabaseConnection() = default;

  /// @brief 获取底层 SOCI session 指针（使用类型擦除避免暴露 SOCI 类型）
  /// @return SOCI session 的指针（void* 类型擦除，避免暴露 SOCI 类型）
  /// @note 此方法使用类型擦除，头文件中不暴露 SOCI 类型
  /// 实现文件需要包含 <soci/session.h> 才能使用
  /// 使用时需要转换为 soci::session&：auto& session = *static_cast<soci::session*>(ptr);
  virtual void* GetSessionPtr() = 0;

  /// @brief 检查连接是否有效
  /// @return 连接有效返回 true，否则返回 false
  virtual bool IsValid() const = 0;

  /// @brief 执行 SQL 查询语句（返回结果集）
  /// @param sql_str SQL SELECT 查询语句
  /// @return 对于 SELECT 查询返回结果集接口指针，失败返回 nullptr
  /// @note 只接受 SELECT 语句，其他语句会返回 nullptr 并记录错误
  ///
  virtual std::unique_ptr<IResultSet> Query(const std::string& sql_str) = 0;

  /// @brief 执行 SQL 语句（非查询，如 INSERT、DELETE、DDL 等）
  /// @param sql_str SQL 语句（非 SELECT 查询）
  /// @return 执行成功返回 true，失败返回 false
  virtual bool Execute(const std::string& sql_str) = 0;

  /// @brief 执行 UPDATE SQL 语句
  /// @param sql_str UPDATE SQL 语句
  /// @return 受影响的行数，失败返回 -1
  virtual int64_t Update(const std::string& sql_str) = 0;

  // ==================== ORM 风格接口（DML 操作） ====================

  /// @brief 插入记录
  /// @param table_name 表名
  /// @param values 字段名到值的映射（值会被转义，防止 SQL 注入）
  /// @return 插入成功返回 true，失败返回 false
  virtual bool Insert(const std::string& table_name, const std::map<std::string, std::string>& values) = 0;

  /// @brief 更新记录
  /// @param table_name 表名
  /// @param values 要更新的字段名到值的映射
  /// @param where_conditions WHERE 条件（字段名到值的映射，多个条件用 AND 连接）
  /// @return 更新的行数，失败返回 -1
  virtual int64_t Update(const std::string& table_name,
                         const std::map<std::string, std::string>& values,
                         const std::map<std::string, std::string>& where_conditions) = 0;

  /// @brief 删除记录
  /// @param table_name 表名
  /// @param where_conditions WHERE 条件（字段名到值的映射，多个条件用 AND 连接）
  /// @note 必须提供 WHERE 条件，防止误删除所有记录
  /// @return 删除的行数，失败返回 -1
  virtual int64_t Delete(const std::string& table_name, const std::map<std::string, std::string>& where_conditions) = 0;

  /// @brief 查询记录（返回结果集接口，支持流式处理）
  /// @param table_name 表名
  /// @param columns 要查询的列名列表（空表示查询所有列 "*"）
  /// @param where_conditions WHERE 条件（字段名到值的映射，多个条件用 AND 连接，最多支持 3 个条件）
  /// @param order_by 排序字段（如 "id DESC" 或 "name ASC"，空表示不排序）
  /// @param limit 限制返回行数（0 表示不限制，设置为 1 可查询单条记录）
  /// @return 结果集接口指针，失败返回 nullptr
  virtual std::unique_ptr<IResultSet> Select(const std::string& table_name,
                                             const std::vector<std::string>& columns = {},
                                             const std::map<std::string, std::string>& where_conditions = {},
                                             const std::string& order_by = "",
                                             std::size_t limit = 0) = 0;

  /// @brief 统计记录数
  /// @param table_name 表名
  /// @param where_conditions WHERE 条件（空表示统计所有记录，最多支持 3 个条件）
  /// @return 记录数，失败返回 -1
  virtual int64_t Count(const std::string& table_name,
                        const std::map<std::string, std::string>& where_conditions = {}) = 0;

  /// @brief 转义
  /// @param sql sql
  /// @return 转义结果，处理特殊字符
  virtual std::string Escape(const std::string& sql) = 0;
};

}  // namespace ugreen::core::database

#endif  // UGREEN_CORE_DATABASE_DATABASE_CONNECTION_HH_
