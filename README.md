# TODO:
1. 互联网上游戏的数据同步/ 帧同步方案
https://www.zhihu.com/question/27765214/answer/3227313427?utm_id=0


##### 1. libuv
基于事件循环的异步 IO 支持;
没有任何事件时, 就会退出 v_run()
特定的事件类型:计时器, IO设备, TCP/UDP, 异步事件等等


##### 2. compile:
- compile libuv test example
mkdir build && cd build
cmake ..
make

cmake -G "Unix Makefiles" -B .
make


- only compile libuv test
```shell
cd external/libuv

To build with CMake:
$ mkdir -p build

$ (cd build && cmake .. -DBUILD_TESTING=ON) # generate project with tests
$ cmake --build build                       # add `-j <n>` with cmake >= 3.12

# Run tests:
$ (cd build && ctest -C Debug --output-on-failure)

# Or manually run tests:
$ build/uv_run_tests                        # shared library build
$ build/uv_run_tests_a                      # static library build
./buidl/uv_run_tests loop_stop              # execute loop_stop test at test-loop-stop.c./buidl/, 在子线程中运行
./buidl/uv_run_tests loop_stop loop_stop # 直接运行, 不创建子线程