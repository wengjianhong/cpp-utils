#ifndef UGREEN_CORE_DATABASE_RESULT_SET_HH_
#define UGREEN_CORE_DATABASE_RESULT_SET_HH_

/// @file result_set.hh
/// @author UGreen NAS Team
/// @brief 数据库查询结果集接口定义
/// @date 2026-01-20
/// @copyright Copyright (c) 2026
/// @details 提供迭代器风格的接口，用于逐行读取查询结果，支持高性能和低内存占用

#include <ugreen/ugos/core/database/row.hh>

namespace ugreen::core::database {

/// @brief 数据库查询结果集接口（流式处理）
/// @details 提供迭代器风格的接口，用于逐行读取查询结果，支持高性能和低内存占用
class IResultSet {
 public:
  virtual ~IResultSet() = default;

  /// @brief 获取下一行，第一次需要先fetch一下才能拿到第一行
  /// @return Row*，如果返回空指针nullptr，表示已是最后一行，没有更多行了
  /// 注意第二次Fetch以后，第一次Fetch获得的指针内容将失效
  virtual Row* Fetch() = 0;
};

}  // namespace ugreen::core::database

#endif  // UGREEN_CORE_DATABASE_RESULT_SET_HH_
