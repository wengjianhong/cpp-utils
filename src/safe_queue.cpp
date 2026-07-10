#include <cpputils/safe_queue.hpp>

#include <string>

namespace cpputils {
// 模板类的显式实例化声明，用于分离编译
// 实际使用时可根据需要添加常用类型的实例化

template class SafeQueue<int>;
template class SafeQueue<long>;
template class SafeQueue<std::string>;

}  // namespace cpputils
