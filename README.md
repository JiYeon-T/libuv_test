##### 1. libuv
基于事件循环的异步 IO 支持;
没有任何事件时, 就会退出 v_run()
特定的事件类型:计时器, IO设备, TCP/UDP, 异步事件等等


##### 2. compile:
mkdir build && cd build
cmake ..
make

cmake -G "Unix Makefiles" -B .
make
