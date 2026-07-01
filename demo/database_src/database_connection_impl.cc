// system headers
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// third-party headers
#include <soci/soci.h>
#include <soci/session.h>

// ugreen headers
#include <ugreen/ugos/core/common/exception.hh>
#include <ugreen/ugos/core/log/logging.hh>

// relative headers
#include "database_connection_impl.hh"
#include "result_set_impl.hh"
#include "connection_pool.hh"

namespace ugreen::core::database {

// ============================================================================
// SQL 通用辅助函数：SQL 语句构建和转义
// ============================================================================

/// 构建 WHERE 子句（通用 SQL 辅助函数）
std::string build_where_clause(const IConnectionPool* pool,
                               const std::map<std::string, std::string>& conditions,
                               soci::session* session) {
  if (conditions.empty()) {
    return "";
  }
  std::ostringstream ss;
  ss << " WHERE ";
  bool first = true;
  for (const auto& kv : conditions) {
    if (!first) {
      ss << " AND ";
    }
    ss << pool->EscapeIdentifier(kv.first, session) << " = :" << kv.first;
    first = false;
  }
  return ss.str();
}

/// 构建 INSERT SQL 语句（ORM 辅助函数）
std::string build_insert_sql(const IConnectionPool* pool,
                             const std::string& table_name,
                             const std::map<std::string, std::string>& values,
                             soci::session* session) {
  std::ostringstream sql;
  sql << "INSERT INTO " << pool->EscapeIdentifier(table_name, session) << " (";
  bool first = true;
  for (const auto& kv : values) {
    if (!first) {
      sql << ", ";
    }
    sql << pool->EscapeIdentifier(kv.first, session);
    first = false;
  }
  sql << ") VALUES (";
  first = true;
  for (const auto& kv : values) {
    if (!first) {
      sql << ", ";
    }
    sql << ":" << kv.first;
    first = false;
  }
  sql << ")";
  return sql.str();
}

// ============================================================================
// 数据库连接实现：DatabaseConnectionImpl
// ============================================================================

DatabaseConnectionImpl::DatabaseConnectionImpl(soci::connection_pool* soci_pool, std::size_t pos, IConnectionPool& pool)
  : valid_(true), pos_(pos), soci_pool_(soci_pool), pool_(&pool) {}

DatabaseConnectionImpl::~DatabaseConnectionImpl() noexcept {
  if (!valid_ || !soci_pool_) {
    return;
  }
  try {
    soci_pool_->give_back(pos_);
  } catch (const soci::soci_error& ex) {
    LOG(ERROR) << "Failed to return connection to pool: " << ex.what();
  }
}

void* DatabaseConnectionImpl::GetSessionPtr() {
  if (!valid_ || !soci_pool_) {
    return nullptr;
  }
  soci::session& session = soci_pool_->at(pos_);
  return static_cast<void*>(&session);
}

bool DatabaseConnectionImpl::IsValid() const {
  return valid_ && soci_pool_ != nullptr;
}

// ============================================================================
// SQL 通用方法：Query, Execute, Update（直接执行 SQL 语句）
// ============================================================================

std::unique_ptr<IResultSet> DatabaseConnectionImpl::Query(const std::string& sql_str) {
  auto& session = *static_cast<soci::session*>(GetSessionPtr());

  try {
    soci::rowset<soci::row> rs((session.prepare << sql_str));
    return std::make_unique<ResultSetImpl>(std::move(rs));
  } catch (const soci::soci_error& ex) {
    LOG(ERROR) << "Query failed: " << ex.what();
    return nullptr;
  }
}

bool DatabaseConnectionImpl::Execute(const std::string& sql_str) {
  auto& session = *static_cast<soci::session*>(GetSessionPtr());

  try {
    session << sql_str;
    return true;
  } catch (const soci::soci_error& ex) {
    LOG(ERROR) << "Execute failed: " << ex.what();
    return false;
  }
}

int64_t DatabaseConnectionImpl::Update(const std::string& sql_str) {
  auto& session = *static_cast<soci::session*>(GetSessionPtr());

  try {
    soci::statement st = (session.prepare << sql_str);
    st.define_and_bind();
    st.execute(true);
    return pool_->GetAffectedRows(st, session);
  } catch (const soci::soci_error& ex) {
    LOG(ERROR) << "Update failed: " << ex.what();
    return -1;
  }
}

/// 执行 SQL 并返回受影响行数（内部函数，用于 Insert/Update/Delete ORM 方法）
int64_t DatabaseConnectionImpl::ExecuteWithParams(const std::string& sql_str,
                                                  const std::map<std::string, std::string>& params) {
  auto& session = *static_cast<soci::session*>(GetSessionPtr());
  soci::statement st = (session.prepare << sql_str);
  for (const auto& kv : params) {
    st.exchange(soci::use(kv.second, kv.first));
  }
  st.define_and_bind();
  st.execute(true);

  // pool_ 在构造函数中总是被设置，不需要检查
  return pool_->GetAffectedRows(st, session);
}

// ============================================================================
// SQL ORM 方法：Insert, Update, Delete, Select, Count（ORM 风格的 CRUD 操作）
// ============================================================================

bool DatabaseConnectionImpl::Insert(const std::string& table_name, const std::map<std::string, std::string>& values) {
  if (values.empty()) {
    return false;
  }

  auto& session = *static_cast<soci::session*>(GetSessionPtr());
  std::string sql = build_insert_sql(pool_, table_name, values, &session);
  try {
    ExecuteWithParams(sql, values);
    return true;
  } catch (const soci::soci_error& ex) {
    LOG(ERROR) << "Insert failed for table '" << table_name << "': " << ex.what();
    return false;
  }
}

int64_t DatabaseConnectionImpl::Update(const std::string& table_name,
                                       const std::map<std::string, std::string>& values,
                                       const std::map<std::string, std::string>& where_conditions) {
  if (values.empty() || where_conditions.empty()) {
    return -1;
  }

  auto& session = *static_cast<soci::session*>(GetSessionPtr());
  std::ostringstream sql;
  sql << "UPDATE " << pool_->EscapeIdentifier(table_name, &session) << " SET ";
  bool first = true;
  std::map<std::string, std::string> params;
  for (const auto& kv : values) {
    if (!first) {
      sql << ", ";
    }
    sql << pool_->EscapeIdentifier(kv.first, &session) << " = :set_" << kv.first;
    params["set_" + kv.first] = kv.second;
    first = false;
  }
  sql << build_where_clause(pool_, where_conditions, &session);
  params.insert(where_conditions.begin(), where_conditions.end());

  try {
    return ExecuteWithParams(sql.str(), params);
  } catch (const soci::soci_error& ex) {
    LOG(ERROR) << "Update failed for table '" << table_name << "': " << ex.what();
    return -1;
  }
}

int64_t DatabaseConnectionImpl::Delete(const std::string& table_name,
                                       const std::map<std::string, std::string>& where_conditions) {
  if (where_conditions.empty()) {
    return -1;
  }

  auto& session = *static_cast<soci::session*>(GetSessionPtr());
  std::ostringstream sql;
  sql << "DELETE FROM " << pool_->EscapeIdentifier(table_name, &session);
  sql << build_where_clause(pool_, where_conditions, &session);

  try {
    return ExecuteWithParams(sql.str(), where_conditions);
  } catch (const soci::soci_error& ex) {
    LOG(ERROR) << "Delete failed for table '" << table_name << "': " << ex.what();
    return -1;
  }
}

std::unique_ptr<IResultSet> DatabaseConnectionImpl::Select(const std::string& table_name,
                                                           const std::vector<std::string>& columns,
                                                           const std::map<std::string, std::string>& where_conditions,
                                                           const std::string& order_by,
                                                           std::size_t limit) {
  auto& session = *static_cast<soci::session*>(GetSessionPtr());
  std::ostringstream sql;
  sql << "SELECT ";
  if (columns.empty()) {
    sql << "*";
  } else {
    bool first = true;
    for (const auto& col : columns) {
      if (!first) {
        sql << ", ";
      }
      sql << pool_->EscapeIdentifier(col, &session);
      first = false;
    }
  }
  sql << " FROM " << pool_->EscapeIdentifier(table_name, &session);
  sql << build_where_clause(pool_, where_conditions, &session);
  if (!order_by.empty()) {
    sql << " ORDER BY " << order_by;
  }
  if (limit > 0) {
    sql << " LIMIT " << limit;
  }

  try {
    soci::rowset<soci::row> rs;
    if (where_conditions.empty()) {
      rs = (session.prepare << sql.str());
    } else {
      auto it = where_conditions.begin();
      switch (where_conditions.size()) {
        case 1: {
          rs = (session.prepare << sql.str(), soci::use(it->second, it->first));
          break;
        }
        case 2: {
          auto it2 = std::next(it);
          rs = (session.prepare << sql.str(), soci::use(it->second, it->first), soci::use(it2->second, it2->first));
          break;
        }
        case 3: {
          auto it2 = std::next(it);
          auto it3 = std::next(it2);
          rs = (session.prepare << sql.str(),
                soci::use(it->second, it->first),
                soci::use(it2->second, it2->first),
                soci::use(it3->second, it3->first));
          break;
        }
        default: {
          UG_THROW_BASE("Too many where conditions (max 3)");
        }
      }
    }
    return std::make_unique<ResultSetImpl>(std::move(rs));
  } catch (const soci::soci_error& ex) {
    LOG(ERROR) << "Select failed for table '" << table_name << "': " << ex.what();
    return nullptr;
  }
}

int64_t DatabaseConnectionImpl::Count(const std::string& table_name,
                                      const std::map<std::string, std::string>& where_conditions) {
  auto& session = *static_cast<soci::session*>(GetSessionPtr());
  std::ostringstream sql;
  sql << "SELECT COUNT(*) FROM " << pool_->EscapeIdentifier(table_name, &session);
  sql << build_where_clause(pool_, where_conditions, &session);

  try {
    int64_t count;
    soci::statement st = (session.prepare << sql.str());
    for (const auto& kv : where_conditions) {
      st.exchange(soci::use(kv.second, kv.first));
    }
    st.exchange(soci::into(count));
    st.define_and_bind();
    st.execute(true);
    return count;
  } catch (const soci::soci_error& ex) {
    LOG(ERROR) << "Count failed for table '" << table_name << "': " << ex.what();
    return -1;
  }
}

}  // namespace ugreen::core::database
