#include <string>

#include "include/safe_queue.hpp"

namespace cpp_utils {
// 模板类的显式实例化声明，用于分离编译
// 实际使用时可根据需要添加常用类型的实例化

template class SafeQueue<int>;
template class SafeQueue<long>;
template class SafeQueue<std::string>;

}  // namespace cpp_utils
