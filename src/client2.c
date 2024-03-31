/**
 * @brief UDS client-server test
 * 
*/
#include <stdio.h>
#include <stdlib.h>
#include "uv.h"



// libuv 的进程与 shell 创建的标准进程通过 UDS 通信
// libuv 创建的 client

/**
 * @brief 
 * 
*/
void start_client(uv_loop_t *loop)
{
    printf("start client\n");
}