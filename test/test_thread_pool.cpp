#include <gtest/gtest.h>
#include <vector>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include "include/thread_pool.hpp"

using namespace cpp_utils;
using namespace std::chrono_literals;

// 基本功能测试
TEST(ThreadPoolTest, BasicFunctionality)
{
    auto pool = ThreadPool::create(2);
    EXPECT_EQ(pool->thread_count(), 2);
    EXPECT_TRUE(pool->is_running());

    // 提交简单任务
    std::atomic<int> counter(0);
    pool->submit([&counter]()
                 { counter++; });
    pool->submit([&counter]()
                 { counter++; });

    // 等待任务完成
    pool->wait_all();
    EXPECT_EQ(counter, 2);
    std::cout << "BasicFunctionality completed." << std::endl;
}

// 带返回值的任务测试
TEST(ThreadPoolTest, TasksWithReturnValues)
{
    auto pool = ThreadPool::create(4);

    // 提交多个带返回值的任务
    auto future1 = pool->submit([]()
                                { return 10; });
    auto future2 = pool->submit([]()
                                { return 20; });
    auto future3 = pool->submit([]()
                                { return 30; });

    // 获取结果
    EXPECT_EQ(future1.get(), 10);
    EXPECT_EQ(future2.get(), 20);
    EXPECT_EQ(future3.get(), 30);
}

// 多线程任务提交测试
TEST(ThreadPoolTest, MultipleThreadsSubmitting)
{
    auto pool = ThreadPool::create(8);
    const int kNumTasks = 10000;
    std::atomic<int> counter(0);
    std::vector<std::thread> submitters;
    std::vector<std::future<void>> futures;
    std::mutex futures_mutex; // 新增：保护futures的并发修改

    while (submitters.size() < 4)
    {
        // 禁止 [&] 捕获 for 循环变量：循环结束后引用悬垂，易表现为 mutex 上的 Resource deadlock avoided
        submitters.emplace_back([&counter, &futures, &futures_mutex, pool]()
                                {
            for (int j = 0; j < kNumTasks; ++j) {
                auto fut = pool->submit([&counter]() { counter++; });
                std::lock_guard<std::mutex> lock(futures_mutex);
                futures.push_back(std::move(fut));
            } });
    }

    // 等待所有提交者完成
    for (auto &t : submitters)
    {
        t.join(); // 无需joinable()检查，线程必为可join状态
    }

    // 等待所有任务完成（future.wait()已足够，无需再调用pool->wait_all()）
    for (auto &fut : futures)
    {
        fut.wait();
    }

    // 验证计数（此时counter必然等于4*kNumTasks）
    EXPECT_EQ(counter, 4 * kNumTasks);
}

// 异常处理测试
TEST(ThreadPoolTest, ExceptionHandling)
{
    auto pool = ThreadPool::create(2);

    // 提交会抛出异常的任务
    auto future = pool->submit([]()
                               { throw std::runtime_error("Test exception"); });

    // 验证异常被正确捕获并传递
    EXPECT_THROW(future.get(), std::runtime_error);

    // 验证线程池仍能正常工作
    std::atomic<bool> completed(false);
    pool->submit([&completed]()
                 { completed = true; });
    pool->wait_all();
    EXPECT_TRUE(completed);
}

// 线程池停止测试
TEST(ThreadPoolTest, StopAndRestart)
{
    auto pool = ThreadPool::create(2);
    EXPECT_TRUE(pool->is_running());

    // 停止线程池
    pool->stop();
    EXPECT_FALSE(pool->is_running());

    // 验证停止后无法提交任务
    EXPECT_THROW(pool->submit([]() {}), std::runtime_error);
}