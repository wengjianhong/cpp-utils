#ifndef UGREEN_CORE_DATABASE_RESULT_SET_IMPL_HH_
#define UGREEN_CORE_DATABASE_RESULT_SET_IMPL_HH_

#include <soci/row.h>
#include <soci/rowset.h>
#include <ugreen/ugos/core/database/result_set.hh>

namespace ugreen::core::database {

/** @brief 查询结果集实现类（流式处理，高性能）
 * @details 封装 SOCI rowset，支持逐行读取，不一次性加载所有数据
 */
class ResultSetImpl : public IResultSet {
 public:
  ResultSetImpl(soci::rowset<soci::row>&& rowset) : rowset_(std::move(rowset)) { current_it_ = rowset_.begin(); }
  virtual ~ResultSetImpl() noexcept override = default;

  Row* Fetch() override {
    if (current_it_ == rowset_.end()) {
      return nullptr;
    } else {
      if (first) {
        first = false;
      } else {
        ++current_it_;
        if (current_it_ == rowset_.end()) {
          return nullptr;
        }
      }
      current_row_.SetRow(&(*current_it_));
      return &current_row_;
    }
  }

 private:
  ResultSetImpl() = delete;
  ResultSetImpl(const ResultSetImpl&) = delete;
  ResultSetImpl& operator=(ResultSetImpl&&) = delete;
  ResultSetImpl& operator=(const ResultSetImpl&) = delete;

  soci::rowset<soci::row> rowset_;
  soci::rowset<soci::row>::iterator current_it_;
  bool first = true;
  mutable Row current_row_;  // 当前行的实现对象（通过指针引用 current_ 指向的 row）
};

}  // namespace ugreen::core::database

#endif  // UGREEN_CORE_DATABASE_RESULT_SET_IMPL_HH_
