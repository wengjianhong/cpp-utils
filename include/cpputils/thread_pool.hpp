/// @file      thread_pool.hpp
/// @brief     基于线程安全队列的通用线程池
/// @details   通过 shared_ptr 工厂创建，支持 submit/wait_all/stop 生命周期管理
/// @author    wengjianhong
/// @date      2026-06-28
/// @copyright CC BY-NC-SA 4.0
#ifndef BASE_COMMON_THREAD_POOL_THREAD_POOL_H_
#define BASE_COMMON_THREAD_POOL_THREAD_POOL_H_

#include <cpputils/safe_queue.hpp>
#include <atomic>
#include <cerrno>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <vector>

namespace cpp_utils {

/// @brief 线程池：基于 SafeQueue 提交任务，支持阻塞等待与优雅停止
class ThreadPool : public std::enable_shared_from_this<ThreadPool> {
 public:
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

  /// @brief 工厂方法：强制通过 shared_ptr 创建，避免栈上对象析构风险
  /// @param thread_count 工作线程数，默认 hardware_concurrency
  /// @return 线程池 shared_ptr
  /// @throws std::invalid_argument thread_count 为 0 时抛出
  static std::shared_ptr<ThreadPool> create(size_t thread_count = std::thread::hardware_concurrency()) {
    if (thread_count == 0) {
      throw std::invalid_argument("Thread count must be greater than 0");
    }
    return std::shared_ptr<ThreadPool>(new ThreadPool(thread_count));
  }

  /// @brief 析构并 stop(true)，确保已提交任务执行完毕
  ~ThreadPool() { stop(true); }

  /// @brief 提交任务并返回 future 获取异步结果
  /// @tparam F 可调用对象类型
  /// @tparam Args 参数类型包
  /// @param f 任务函数
  /// @param args 绑定到 f 的参数
  /// @return 任务结果的 std::future
  /// @throws std::runtime_error 线程池已停止时抛出
  template <typename F, typename... Args>
  auto submit(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
    if (!is_running_.load(std::memory_order_acquire)) {
      throw std::runtime_error("ThreadPool is stopped");
    }

    using ReturnType = typename std::result_of<F(Args...)>::type;
    auto task =
      std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    std::future<ReturnType> result = task->get_future();

    task_count_.fetch_add(1, std::memory_order_acq_rel);
    auto self = shared_from_this();

    task_queue_.push([task, self]() {
      try {
        if (task) {
          (*task)();
        }
      } catch (const std::exception& e) {
        std::cerr << "Task error: " << e.what() << std::endl;
      }

      size_t remaining = self->task_count_.fetch_sub(1, std::memory_order_acq_rel);
      if (remaining == 1) {
        std::lock_guard<std::mutex> lock(self->wait_mutex_);
        self->wait_cv_.notify_all();
      }
    });

    return result;
  }

  /// @brief 阻塞等待所有已提交任务完成（task_count_ 归零）
  void wait_all() {
    std::unique_lock<std::mutex> lock(wait_mutex_);
    wait_cv_.wait(lock, [this]() { return task_count_.load(std::memory_order_acquire) == 0; });
  }

  /// @brief 获取当前未完成任务数（含已出队未执行）
  /// @return 近似任务计数
  size_t pending_tasks() const { return task_count_.load(std::memory_order_acquire); }

  /// @brief 获取工作线程数量
  /// @return 线程数
  size_t thread_count() const { return threads_.size(); }

  /// @brief 停止线程池并 join 工作线程
  /// @param wait_for_completion true 时等待队列中任务执行完毕；false 时清空未执行任务
  void stop(bool wait_for_completion = true) {
    if (!is_running_.exchange(false, std::memory_order_acq_rel)) {
      return;
    }

    if (!wait_for_completion) {
      clear_queue();
    }

    if (wait_for_completion) {
      wait_all();
    }

    for (auto& thread : threads_) {
      if (thread.joinable()) {
        thread.join();
      }
    }
    threads_.clear();
  }

  /// @brief 线程池是否处于运行状态
  /// @return true 表示可继续 submit
  bool is_running() const { return is_running_.load(std::memory_order_acquire); }

 private:
  /// @brief 私有构造，仅允许 create() 创建
  /// @param thread_count 工作线程数
  explicit ThreadPool(size_t thread_count) : wait_for_completion_(true), is_running_(true), task_count_(0) {
    threads_.reserve(thread_count);
    for (size_t i = 0; i < thread_count; ++i) {
      threads_.emplace_back(&ThreadPool::worker_thread, this);
    }
  }

  /// @brief 丢弃队列中未执行任务并递减计数
  void clear_queue() {
    std::function<void()> dummy;
    while (task_queue_.pop(dummy)) {
      task_count_.fetch_sub(1, std::memory_order_acq_rel);
    }
    std::lock_guard<std::mutex> lock(wait_mutex_);
    wait_cv_.notify_all();
  }

  /// @brief 工作线程主循环：从队列取任务执行
  void worker_thread() {
    while (is_running_.load(std::memory_order_acquire)) {
      std::function<void()> task;
      if (task_queue_.pop(task)) {
        try {
          if (task) {
            task();
          }
        } catch (const std::exception& e) {
          std::cerr << "[WorkerThread] Task execution error: " << e.what() << std::endl;
        } catch (...) {
          std::cerr << "[WorkerThread] Unknown task error" << std::endl;
        }
      } else {
        std::this_thread::yield();
      }
    }

    if (wait_for_completion_) {
      std::function<void()> remaining_task;
      while (task_queue_.pop(remaining_task)) {
        try {
          if (remaining_task) {
            remaining_task();
          }
        } catch (...) {
        }
      }
    }
  }

  bool wait_for_completion_;                     ///< stop 时是否 drain 剩余任务
  std::atomic<bool> is_running_;                 ///< 运行标志
  std::atomic<size_t> task_count_;               ///< 未完成任务计数
  std::mutex wait_mutex_;                        ///< wait_all 互斥锁
  std::condition_variable wait_cv_;              ///< wait_all 条件变量
  std::vector<std::thread> threads_;             ///< 工作线程列表
  SafeQueue<std::function<void()>> task_queue_;  ///< 任务队列
};
}  // namespace cpp_utils

#endif  // BASE_COMMON_THREAD_POOL_THREAD_POOL_H_
