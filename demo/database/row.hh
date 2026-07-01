#ifndef UGREEN_CORE_DATABASE_ROW_HH_
#define UGREEN_CORE_DATABASE_ROW_HH_

/// @file row.hh
/// @author UGreen NAS Team
/// @brief 数据库行接口定义
/// @date 2026-01-20
/// @copyright Copyright (c) 2026
/// @details 提供对单行数据的访问接口，支持按列名或下标访问字段值

#include <cstddef>
#include <string>
#include <sstream>
#include <optional>
#include <soci/row.h>
#include <ugreen/ugos/core/common/exception.hh>

/// @brief 数据库相关功能模块
namespace ugreen::core::database {

inline std::string soci_db_type_to_str(soci::data_type dt) {
  switch (dt) {
    case soci::dt_string:
      return "db_string";
    case soci::dt_date:
      return "db_date";
    case soci::dt_double:
      return "db_double";
    case soci::dt_integer:
      return "db_int32";
    case soci::dt_long_long:
      return "db_int64";
    case soci::dt_unsigned_long_long:
      return "db_uint64";
    case soci::dt_blob:
      return "db_blob";
    case soci::dt_xml:
      return "db_xml";
  }
  // unreachable
  return "unknown " + std::to_string(static_cast<int>(dt));
}
/// @brief database exception
class DatabaseException : public ugreen::core::common::Exception {
 public:
  using ugreen::core::common::Exception::Exception;
};

/// @brief DatabaseBadCast exception
class DatabaseBadCast : public DatabaseException {
 public:
  using DatabaseException::DatabaseException;
};

/// @brief 数据库行接口（单行数据访问）
/// @details 提供对单行数据的访问接口，支持按列名或下标访问字段值
///
class Row {
 public:
  Row() : row_(nullptr) {}
  virtual ~Row() = default;

  /// @brief 按字段名获取指定字段值（模板方法）
  /// @tparam T 返回类型，支持 std::string, int, int64_t, double, bool 等
  /// @param field_name 字段名（std::string）
  /// @return 字段值，自动转换为目标类型
  /// @note 使用示例：
  ///       auto name = row->Get<std::string>("name");
  ///       auto email = row->Get<std::string>("email");
  template <typename T>
  std::optional<T> Get(const std::string& field_name) const {
    if (!row_) {
      UG_THROW(DatabaseException, "Row is not initialized");
    }
    if (row_->get_indicator(field_name) != soci::i_ok) {
      return std::optional<T>();
    }
    try {
      std::optional<T> ret(std::move(row_->get<T>(field_name)));
      return ret;
    } catch (std::bad_cast& ex) {
      UG_THROW(DatabaseBadCast, getBadCastString(field_name, typeid(T).name(), ex));
    } catch (std::exception& ex) {
      UG_THROW(DatabaseException, std::string("row get by field name ") + field_name + " fail " + ex.what());
    }
  }

  /// @brief 按下标获取指定字段值（模板方法）
  /// @tparam T 返回类型，支持 std::string, int, int64_t, double, bool 等
  /// @param column_index 列索引（std::size_t，从 0 开始）
  /// @return 字段值，自动转换为目标类型
  /// @note 使用示例：
  ///       auto age = row->Get<int>(0);
  template <typename T>
  std::optional<T> Get(std::size_t column_index) const {
    if (!row_) {
      UG_THROW(DatabaseException, "Row is not initialized");
    }
    if (column_index >= row_->size()) {
      UG_THROW(DatabaseException, "Column index out of range: " + std::to_string(column_index));
    }
    if (row_->get_indicator(column_index) != soci::i_ok) {
      return std::optional<T>();
    }
    try {
      std::optional<T> ret(std::move(row_->get<T>(column_index)));
      return ret;
    } catch (std::bad_cast& ex) {
      UG_THROW(DatabaseBadCast, getBadCastString(column_index, typeid(T).name(), ex));
    } catch (std::exception& ex) {
      UG_THROW(DatabaseException,
               std::string("row get by index ") + std::to_string(column_index) + " fail " + ex.what());
    }
  }

  void SetRow(const soci::row* row) { row_ = row; }

 private:
  template <typename CT>
  std::string getBadCastString(const CT& column, const char* type_name, std::bad_cast& ex) const {
    soci::column_properties const& props = row_->get_properties(column);
    soci::data_type db_type = props.get_data_type();
    std::stringstream ss;
    ss << "badcast from " << soci_db_type_to_str(db_type) << " to " << type_name << " on " << column
       << ", exception: " << ex.what();
    return ss.str();
  }

  Row(const Row&) = delete;
  Row(Row&&) = delete;
  Row& operator=(const Row&) = delete;
  Row& operator=(Row&&) = delete;
  const soci::row* row_;
};
}  // namespace ugreen::core::database

#endif  // UGREEN_CORE_DATABASE_ROW_HH_
