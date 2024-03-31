#include <stdio.h>
#include <stdlib.h>
#include "uv.h"


#define FIXTURE "testfile"

uv_loop_t *loop;

static uv_fs_poll_t poll_handle;
static uv_timer_t timer_handle;
static uv_timer_t timer_handle2;
static int timer_called = 0;
static int idle_cnt = 0;

static void timer_cb(uv_timer_t* handle) {
//   ASSERT_PTR_EQ(handle, &timer_handle);
    timer_called = 1;
    printf("timer_cb\n");
}


int main(int argc ,char *argv[])
{
    printf("lib uv version:%s\n", uv_version_string());
    loop = malloc(sizeof(uv_loop_t));
    // uv_idle_t idler;
    uv_loop_init(loop);

    uv_timer_init(loop, &timer_handle);
    uv_timer_start(&timer_handle, timer_cb, 5000, 5000);

    // uv_timer_init(loop, &timer_handle2);
    // uv_timer_start(&timer_handle2, timer_cb2, 5000, 5000);

    // uv_fs_poll_init(loop, &poll_handle);

    // uv_fs_poll_start(&poll_handle, poll_cb_fail, FIXTURE, 1000);

    // uv_idle_init(loop, &idler);
    // uv_idle_start(&idler, idle_cb);

    start_server1(loop);


    uv_run(loop, UV_RUN_DEFAULT);
    printf("Now quitting.\n");

    printf("alive:%d\n", uv_loop_alive(loop));
    uv_loop_close(loop);
    printf("alive:%d\n", uv_loop_alive(loop));
    free(loop);

    return 0;
}

uv_loop_t *get_loop(void)
{
    return loop;
}