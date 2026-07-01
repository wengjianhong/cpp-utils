#include <cpputils/safe_queue.hpp>

#include <atomic>
#include <gtest/gtest.h>
#include <thread>
#include <vector>

using namespace cpp_utils;

// 基本功能测试
TEST(SafeQueueTest, BasicOperations) {
  SafeQueue<int> queue;

  // 初始状态应为空
  EXPECT_TRUE(queue.empty());

  // 入队操作
  queue.push(10);
  queue.push(20);
  EXPECT_FALSE(queue.empty());

  // 出队操作
  int value;
  EXPECT_TRUE(queue.pop(value));
  EXPECT_EQ(value, 10);

  EXPECT_TRUE(queue.pop(value));
  EXPECT_EQ(value, 20);

  // 队列应为空
  EXPECT_FALSE(queue.pop(value));
  EXPECT_TRUE(queue.empty());
}

// 移动语义测试
TEST(SafeQueueTest, MoveSemantics) {
  SafeQueue<std::string> queue;
  std::string str = "test string";

  // 测试移动入队
  queue.push(std::move(str));
  EXPECT_TRUE(str.empty());  // 原字符串应被移动

  // 测试移动出队
  std::string result;
  EXPECT_TRUE(queue.pop(result));
  EXPECT_EQ(result, "test string");
}

// 多生产者单消费者测试
TEST(SafeQueueTest, MultipleProducers) {
  SafeQueue<int> queue;
  const int kNumProducers = 4;
  const int kItemsPerProducer = 1000;
  std::vector<std::thread> producers;

  // 启动多个生产者线程
  for (int i = 0; i < kNumProducers; ++i) {
    producers.emplace_back([&queue, i, kItemsPerProducer]() {
      for (int j = 0; j < kItemsPerProducer; ++j) {
        queue.push(i * kItemsPerProducer + j);
      }
    });
  }

  // 等待所有生产者完成
  for (auto &t : producers) {
    t.join();
  }

  // 单消费者消费所有元素
  std::vector<int> results;
  int value;
  while (queue.pop(value)) {
    results.push_back(value);
  }

  // 验证元素总数正确
  EXPECT_EQ(results.size(), kNumProducers * kItemsPerProducer);

  // 验证所有元素都被正确生产和消费
  std::sort(results.begin(), results.end());
  for (unsigned int i = 0; i < results.size(); ++i) {
    EXPECT_EQ(results[i], i);
  }
}

// 多生产者多消费者测试
TEST(SafeQueueTest, MultipleProducersConsumers) {
  SafeQueue<int> queue;
  const int kNumProducers = 4;
  const int kNumConsumers = 2;
  const int kItemsPerProducer = 1000;
  std::vector<std::thread> producers;
  std::vector<std::thread> consumers;
  std::atomic<int> total_consumed(0);
  std::atomic<bool> done(false);

  // 启动生产者
  for (int i = 0; i < kNumProducers; ++i) {
    producers.emplace_back([&queue, i, kItemsPerProducer]() {
      for (int j = 0; j < kItemsPerProducer; ++j) {
        queue.push(i * kItemsPerProducer + j);
      }
    });
  }

  // 启动消费者
  for (int i = 0; i < kNumConsumers; ++i) {
    consumers.emplace_back([&queue, &total_consumed, &done]() {
      int value;
      while (!done.load() || !queue.empty()) {
        if (queue.pop(value)) {
          total_consumed.fetch_add(1, std::memory_order_relaxed);
        } else {
          std::this_thread::yield();  // 避免忙等待
        }
      }
    });
  }

  // 等待生产者完成
  for (auto &t : producers) {
    t.join();
  }

  // 标记生产完成
  done.store(true);

  // 等待消费者完成
  for (auto &t : consumers) {
    t.join();
  }

  // 验证所有元素都被消费
  EXPECT_EQ(total_consumed.load(), kNumProducers * kItemsPerProducer);
}