/// @file      safe_queue.hpp
/// @brief     线程安全的有锁队列（多生产者-多消费者）
/// @details   支持非阻塞 pop 与阻塞 block_pop，禁止拷贝/移动
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef BASE_COMMON_SAFE_QUEUE_H_
#define BASE_COMMON_SAFE_QUEUE_H_

#include <deque>
#include <mutex>
#include <utility>
#include <cstddef>
#include <stdexcept>
#include <condition_variable>

namespace cpp_utils {

/// @brief 线程安全队列，底层 deque + mutex + condition_variable
/// @tparam T 元素类型
template <typename T>
class SafeQueue {
 public:
  SafeQueue() = default;
  ~SafeQueue() = default;

  SafeQueue(const SafeQueue&) = delete;
  SafeQueue& operator=(const SafeQueue&) = delete;
  SafeQueue(SafeQueue&&) = delete;
  SafeQueue& operator=(SafeQueue&&) = delete;

  /// @brief 左值入队（拷贝）
  /// @param value 待入队元素
  void push(const T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    data_.push_back(value);
    cv_.notify_one();
  }

  /// @brief 右值入队（移动）
  /// @param value 待移动入队的元素
  void push(T&& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    data_.push_back(std::move(value));
    cv_.notify_one();
  }

  /// @brief 非阻塞出队
  /// @param value 出队结果写入此引用
  /// @return 队列为空返回 false，否则 true
  bool pop(T& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (data_.empty()) {
      return false;
    }

    value = std::move(data_.front());
    data_.pop_front();
    return true;
  }

  /// @brief 阻塞出队，队列为空时等待直至有数据
  /// @param value 出队结果写入此引用
  /// @return 成功取到元素时恒为 true
  bool block_pop(T& value) {
    std::unique_lock<std::mutex> lock(mutex_);

    while (data_.empty()) {
      cv_.wait(lock);
    }

    value = std::move(data_.front());
    data_.pop_front();
    return true;
  }

  /// @brief 获取队列当前元素个数
  /// @return 瞬时 size（高并发下可能略有偏差）
  size_t size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return data_.size();
  }

  /// @brief 判断队列是否为空
  /// @return 瞬时 empty 状态
  bool empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return data_.empty();
  }

  /// @brief 清空队列
  void clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    data_.clear();
  }

  /// @brief 批量入队
  /// @tparam InputIt 输入迭代器类型
  /// @param first 区间起始
  /// @param last 区间结束（不含）
  template <typename InputIt>
  void push_bulk(InputIt first, InputIt last) {
    std::lock_guard<std::mutex> lock(mutex_);
    data_.insert(data_.end(), first, last);
    if (data_.size() >= 1) {
      cv_.notify_all();
    }
  }

 private:
  mutable std::mutex mutex_;    ///< 保护临界区
  std::condition_variable cv_;  ///< 空队列阻塞等待
  std::deque<T> data_;          ///< 底层存储
};

}  // namespace cpp_utils

#endif  // BASE_COMMON_SAFE_QUEUE_H_
