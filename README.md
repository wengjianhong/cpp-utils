# cpputils

C++ 通用工具库：线程池、安全队列、SOCI 数据库封装等。

安装前缀由用户在 configure / install 时通过 `-DCMAKE_INSTALL_PREFIX` 指定，本仓库 CMakeLists 无需修改。

## 环境要求

- C++20 及以上
- CMake 3.22+
- SOCI（可选，`CPPUTILS_ENABLE_SOCI=ON` 默认开启）
- 操作系统：Linux（推荐 Ubuntu 24.04+）

## 编译安装

### 方式一：默认位置（/usr/local）

```shell
git clone git@github.com:wengjianhong/cpputils.git
cd cpputils

# 编译安装
cmake -B build
cmake --build build -j $(($(nproc)/4))
sudo cmake --install build
sudo ldconfig
```

安装布局：

```
/usr/local/
├── lib/libcpputils.so
├── lib/cmake/cpputils/
└── include/cpputils/
```

### 方式二：目录隔离（/usr/local/cpputils）

```shell
# 编译安装
cmake -B build -DCMAKE_INSTALL_PREFIX=/usr/local/cpputils
cmake --build build -j $(($(nproc)/4))
sudo cmake --install build

# 配置ldconfig，注册运行期库路径
echo /usr/local/cpputils/lib | sudo tee /etc/ld.so.conf.d/cpputils.conf
sudo ldconfig
```

安装布局：

```
/usr/local/cpputils/
├── lib/libcpputils.so
├── lib/cmake/cpputils/
└── include/cpputils/
```

> 若曾用方式一安装，切换到方式二前建议清理旧包：
> `sudo rm -rf /usr/local/lib/cmake/cpputils /usr/local/lib/libcpputils.so`

## 下游项目使用

CMake 中链接：

```cmake
find_package(cpputils 0.1.0 REQUIRED CONFIG)
target_link_libraries(my_app PRIVATE cpputils::cpputils)
```

| 安装方式 | configure 额外参数 |
|----------|-------------------|
| 方式一（默认 /usr/local） | 无 |
| 方式二（/usr/local/cpputils） | `-DCMAKE_PREFIX_PATH=/usr/local/cpputils` |

运行期需能加载 `libcpputils.so`：方式一通常 `ldconfig` 即可；方式二须注册 `/usr/local/cpputils/lib`（见上文）。
