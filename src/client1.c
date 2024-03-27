/**
 * @file UNIX domain socket test
 * 
*/
#include <stdio.h>
#include <stdlib.h>
#include "uv.h"


#define UNIX_DOMAIN_SOCKET_PATH     "/home/qz/code/LinuxDriverDevelopment/foo.test"

static uv_loop_t loop;
static uv_timer_t timer;
static uv_pipe_t server_channel;


static void timer_cb(uv_timer_t* handle);
static void listen_cb(uv_stream_t* handle, int status);

void start_server(void)
{
    int r;
    uv_loop_init(&loop);

    uv_timer_init(&loop, &timer);
    uv_timer_start(&timer, timer_cb, 0, 2000);

    uv_pipe_init(&loop, &server_channel, 0);
    ASSERT_OK(r);
    uv_pipe_bind(&server_channel, UNIX_DOMAIN_SOCKET_PATH);
    ASSERT_OK(r);
    uv_listen((uv_stream_t*)&server_channel, 2, )

    uv_run(&loop, UV_RUN_DEFAULT);
}

static void timer_cb(uv_timer_t* handle)
{
    printf("%s\n", __func__);
}

static void listen_cb(uv_stream_t* handle, int status)
{
  int r;
  ASSERT_OK(status);

  r = uv_accept((uv_stream_t*)&server_channel, (uv_stream_t*)&ctx2.channel);
  ASSERT_OK(r);

  send_recv_start();
}