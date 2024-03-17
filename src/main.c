#include <stdio.h>
#include <stdlib.h>
#include "uv.h"


#define FIXTURE "testfile"


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

static void timer_cb2(uv_timer_t* handle) {
//   ASSERT_PTR_EQ(handle, &timer_handle);
    timer_called = 1;
    printf("timer_cb2\n");
}

static void poll_cb_fail(uv_fs_poll_t* handle,
                         int status,
                         const uv_stat_t* prev,
                         const uv_stat_t* curr) {
//   ASSERT(0 && "fail_cb called");
    printf("fail_cb called status:%d\n", status);
}

static void idle_cb(uv_idle_t *handle)
{
    ++idle_cnt;
    printf("idle:%d\n", idle_cnt);
    if (idle_cnt > 5) {
        printf("stop idle\n");
        uv_idle_stop(handle);
    }
}

int main(int argc ,char *argv[])
{
    printf("lib uv version:%s\n", uv_version_string());
    uv_loop_t *loop = malloc(sizeof(uv_loop_t));
    uv_idle_t idler;
    uv_loop_init(loop);

    uv_timer_init(loop, &timer_handle);
    uv_timer_start(&timer_handle, timer_cb, 1000, 1000);

    uv_timer_init(loop, &timer_handle2);
    uv_timer_start(&timer_handle2, timer_cb2, 5000, 5000);

    uv_fs_poll_init(loop, &poll_handle);

    uv_fs_poll_start(&poll_handle, poll_cb_fail, FIXTURE, 1000);

    uv_idle_init(loop, &idler);
    uv_idle_start(&idler, idle_cb);

    printf("Now quitting.\n");
    uv_run(loop, UV_RUN_DEFAULT);

    printf("alive:%d\n", uv_loop_alive(loop));
    uv_loop_close(loop);
    printf("alive:%d\n", uv_loop_alive(loop));
    free(loop);

    return 0;
}