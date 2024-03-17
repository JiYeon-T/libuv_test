/* Copyright libuv project contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "uv.h"
#include "task.h"
#include <string.h> /* memset */

#define UV_NS_TO_MS 1000000

typedef struct {
  uv_fs_t open_req;
  uv_fs_t write_req;
  uv_fs_t close_req;
} fs_reqs_t;

static uint64_t last_events_count;
static char test_buf[] = "test-buffer\n";
static fs_reqs_t fs_reqs;
static int pool_events_counter;


static void timer_spin_cb(uv_timer_t* handle) {
  uint64_t t;
  printf("TIMER_SPIN_CB loop:%X\n", handle->loop);
  (*(int*) handle->data)++;
  t = uv_hrtime();
  /* Spin for 500 ms to spin loop time out of the delta check. */
  while (uv_hrtime() - t < 600 * UV_NS_TO_MS) { }
}


TEST_IMPL(metrics_idle_time) {
#if defined(__OpenBSD__)
  RETURN_SKIP("Test does not currently work in OpenBSD");
#endif
  const uint64_t timeout = 1000;
  uv_timer_t timer;
  uint64_t idle_time;
  int cntr;

  cntr = 0;
  timer.data = &cntr;

  ASSERT_OK(uv_loop_configure(uv_default_loop(), UV_METRICS_IDLE_TIME));
  ASSERT_OK(uv_timer_init(uv_default_loop(), &timer));
  ASSERT_OK(uv_timer_start(&timer, timer_spin_cb, timeout, 0));

  ASSERT_OK(uv_run(uv_default_loop(), UV_RUN_DEFAULT));
  ASSERT_GT(cntr, 0);

  idle_time = uv_metrics_idle_time(uv_default_loop());
  printf("idle time:%ldns\n", idle_time); // 单位是 ns?

  /* Permissive check that the idle time matches within the timeout ±500 ms. */
  ASSERT_LE(idle_time, (timeout + 500) * UV_NS_TO_MS);
  ASSERT_GE(idle_time, (timeout - 500) * UV_NS_TO_MS);

  MAKE_VALGRIND_HAPPY(uv_default_loop());
  return 0;
}


static void metrics_routine_cb(void* arg) {
  const uint64_t timeout = 1000;
  uv_loop_t loop;
  uv_timer_t timer;
  uint64_t idle_time;
  int cntr;

  cntr = 0;
  timer.data = &cntr;

  ASSERT_OK(uv_loop_init(&loop));
  printf("create loop:%X\n", &loop);
  ASSERT_OK(uv_loop_configure(&loop, UV_METRICS_IDLE_TIME));
  ASSERT_OK(uv_timer_init(&loop, &timer));
  ASSERT_OK(uv_timer_start(&timer, timer_spin_cb, timeout, 0));

  ASSERT_OK(uv_run(&loop, UV_RUN_DEFAULT));
  ASSERT_GT(cntr, 0);

  idle_time = uv_metrics_idle_time(&loop);

  /* Only checking that idle time is greater than the lower bound since there
   * may have been thread contention, causing the event loop to be delayed in
   * the idle phase longer than expected.
   */
  ASSERT_GE(idle_time, (timeout - 500) * UV_NS_TO_MS);

  close_loop(&loop);
  ASSERT_OK(uv_loop_close(&loop));
}


TEST_IMPL(metrics_idle_time_thread) {
  uv_thread_t threads[5];
  int i;

  printf("main enter\n");
  for (i = 0; i < 5; i++) {
    ASSERT_OK(uv_thread_create(&threads[i], metrics_routine_cb, NULL));
  }

  for (i = 0; i < 5; i++) {
    uv_thread_join(&threads[i]);
  }
  printf("main exit\n");
  return 0;
}


static void timer_noop_cb(uv_timer_t* handle) {
  printf("TIMER_SPIN_CB loop:%X\n", handle->loop);
  (*(int*) handle->data)++; // 修改 handle->data 指向的变量:  int cntr;
}


TEST_IMPL(metrics_idle_time_zero) {
  uv_metrics_t metrics;
  uv_timer_t timer;
  int cntr;

  cntr = 0;
  timer.data = &cntr; // 设置 timer->data 作为参数传入, 在回调函数中修改
  ASSERT_OK(uv_loop_configure(uv_default_loop(), UV_METRICS_IDLE_TIME));
  ASSERT_OK(uv_timer_init(uv_default_loop(), &timer));
  ASSERT_OK(uv_timer_start(&timer, timer_noop_cb, 0, 0));

  ASSERT_OK(uv_run(uv_default_loop(), UV_RUN_DEFAULT));

  ASSERT_GT(cntr, 0);
  ASSERT_OK(uv_metrics_idle_time(uv_default_loop()));

  ASSERT_OK(uv_metrics_info(uv_default_loop(), &metrics));
  ASSERT_UINT64_EQ(cntr, metrics.loop_count);
  printf("cntr:%d loop_count:%d\n", cntr, metrics.loop_count);

  MAKE_VALGRIND_HAPPY(uv_default_loop());
  return 0;
}


static void close_cb(uv_fs_t* req) {
  uv_metrics_t metrics;
  ASSERT_OK(uv_metrics_info(uv_default_loop(), &metrics));
  ASSERT_UINT64_EQ(3, metrics.loop_count); // 第三步关闭
  ASSERT_UINT64_GT(metrics.events, last_events_count);
  printf("%s loop_cnt:%d\n", __func__, metrics.loop_count);

  uv_fs_req_cleanup(req);
  last_events_count = metrics.events;
}


static void write_cb(uv_fs_t* req) {
  uv_metrics_t metrics;

  ASSERT_OK(uv_metrics_info(uv_default_loop(), &metrics));
  ASSERT_UINT64_EQ(2, metrics.loop_count); // 第二步写文件, 发出关闭
  ASSERT_UINT64_GT(metrics.events, last_events_count);
  ASSERT_EQ(req->result, sizeof(test_buf));
  printf("%s loop_cnt:%d\n", __func__, metrics.loop_count);

  uv_fs_req_cleanup(req);
  last_events_count = metrics.events;

  ASSERT_OK(uv_fs_close(uv_default_loop(),
                        &fs_reqs.close_req,
                        fs_reqs.open_req.result,
                        close_cb));
}


static void create_cb(uv_fs_t* req) {
  uv_metrics_t metrics;

  ASSERT_OK(uv_metrics_info(uv_default_loop(), &metrics));
  /* Event count here is still 0 so not going to check. */
  ASSERT_UINT64_EQ(1, metrics.loop_count); // 第一步创建文件
  ASSERT_GE(req->result, 0);
  printf("%s loop_cnt:%d\n", __func__, metrics.loop_count);

  uv_fs_req_cleanup(req);
  last_events_count = metrics.events;

  uv_buf_t iov = uv_buf_init(test_buf, sizeof(test_buf)); //异步写文件
  ASSERT_OK(uv_fs_write(uv_default_loop(),
                        &fs_reqs.write_req,
                        req->result,
                        &iov,
                        1,
                        0,
                        write_cb));
}

static void unlink_cb(uv_fs_t* req)
{
  printf("file unlinked\n");
}


static void prepare_cb(uv_prepare_t* handle) {
  uv_metrics_t metrics;

  uv_prepare_stop(handle);

  ASSERT_OK(uv_metrics_info(uv_default_loop(), &metrics));
  ASSERT_UINT64_EQ(0, metrics.loop_count); // 第 0 步创建爱呢文件
  ASSERT_UINT64_EQ(0, metrics.events);
  printf("%s loop_cnt:%d\n", __func__, metrics.loop_count);

  ASSERT_OK(uv_fs_open(uv_default_loop(),
                       &fs_reqs.open_req,
                       "test_file",
                       O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR,
                       create_cb));
}

// 测试步骤:
//prepare -> prepare_cb -> 创建文件
// create_cb -> 写文件
// write_cb -> 关闭文件
// close_cb -> loop 退出
TEST_IMPL(metrics_info_check) {
  uv_fs_t unlink_req;
  uv_prepare_t prepare;

  printf("main enter\n");
  uv_fs_unlink(NULL, &unlink_req, "test_file", NULL);
  uv_fs_req_cleanup(&unlink_req);

  ASSERT_OK(uv_prepare_init(uv_default_loop(), &prepare));
  ASSERT_OK(uv_prepare_start(&prepare, prepare_cb));

  ASSERT_OK(uv_run(uv_default_loop(), UV_RUN_DEFAULT));
  printf("loop exit\n");
  uv_fs_unlink(NULL, &unlink_req, "test_file", NULL);
  //如果要设置 callback() 就要设置 loop() 否则会 dump
  // uv_fs_unlink(&loop_name, &unlink_req, "test_file", unlink_cb);
  uv_fs_req_cleanup(&unlink_req);

  MAKE_VALGRIND_HAPPY(uv_default_loop());
  printf("main exit\n");
  return 0;
}


static void fs_prepare_cb(uv_prepare_t* handle) {
  uv_metrics_t metrics;

  ASSERT_OK(uv_metrics_info(uv_default_loop(), &metrics));
  printf("%s pool_events_counter:%d\n", __func__, pool_events_counter);
  if (pool_events_counter == 1)
    ASSERT_EQ(metrics.events, metrics.events_waiting);

  if (pool_events_counter < 7)
    return;

  uv_prepare_stop(handle); // prepare stop 后没有任何需要等待的异步事件, loop 停止
  pool_events_counter = -42;
}


static void fs_stat_cb(uv_fs_t* req) {
  printf("%s pool_events_counter:%d\n", __func__, pool_events_counter);
  uv_fs_req_cleanup(req);
  pool_events_counter++;
}


static void fs_work_cb(uv_work_t* req) {
    printf("%s pool_events_counter:%d\n", __func__, pool_events_counter);
}


static void fs_after_work_cb(uv_work_t* req, int status) {
  printf("%s pool_events_counter:%d\n", __func__, pool_events_counter);
  free(req);
  pool_events_counter++;
}


static void fs_write_cb(uv_fs_t* req) {
  printf("%s pool_events_counter:%d\n", __func__, pool_events_counter);
  //这种写法
  uv_work_t* work1 = malloc(sizeof(*work1));
  uv_work_t* work2 = malloc(sizeof(*work2));
  pool_events_counter++;

  uv_fs_req_cleanup(req);

  ASSERT_OK(uv_queue_work(uv_default_loop(),
                          work1,
                          fs_work_cb,
                          fs_after_work_cb));
  ASSERT_OK(uv_queue_work(uv_default_loop(),
                          work2,
                          fs_work_cb,
                          fs_after_work_cb));
}


static void fs_random_cb(uv_random_t* req, int status, void* buf, size_t len) {
  printf("%s pool_events_counter:%d\n", __func__, pool_events_counter);
  pool_events_counter++;
}


static void fs_addrinfo_cb(uv_getaddrinfo_t* req,
                           int status,
                           struct addrinfo* res) {
  printf("%s pool_events_counter:%d\n", __func__, pool_events_counter);
  uv_freeaddrinfo(req->addrinfo);
  pool_events_counter++;
}


TEST_IMPL(metrics_pool_events) {
  uv_buf_t iov;
  uv_fs_t open_req;
  uv_fs_t stat1_req;
  uv_fs_t stat2_req;
  uv_fs_t unlink_req;
  uv_fs_t write_req;
  uv_getaddrinfo_t addrinfo_req;
  uv_metrics_t metrics;
  uv_prepare_t prepare;
  uv_random_t random_req;
  int fd;
  char rdata;

  ASSERT_OK(uv_loop_configure(uv_default_loop(), UV_METRICS_IDLE_TIME));

  uv_fs_unlink(NULL, &unlink_req, "test_file", NULL);
  uv_fs_req_cleanup(&unlink_req);

  ASSERT_OK(uv_prepare_init(uv_default_loop(), &prepare));
  ASSERT_OK(uv_prepare_start(&prepare, fs_prepare_cb));

  //在 loop 外进行的操作都要 cleanup_req??
  pool_events_counter = 0;
  fd = uv_fs_open(NULL,
                  &open_req,
                  "test_file",
                  O_WRONLY | O_CREAT,
                  S_IRUSR | S_IWUSR,
                  NULL);
  ASSERT_GT(fd, 0);
  uv_fs_req_cleanup(&open_req);

  iov = uv_buf_init(test_buf, sizeof(test_buf));
  ASSERT_OK(uv_fs_write(uv_default_loop(),
                        &write_req,
                        fd,
                        &iov,
                        1,
                        0,
                        fs_write_cb));
  ASSERT_OK(uv_fs_stat(uv_default_loop(),
                       &stat1_req,
                       "test_file",
                       fs_stat_cb));
  ASSERT_OK(uv_fs_stat(uv_default_loop(),
                       &stat2_req,
                       "test_file",
                       fs_stat_cb));
  ASSERT_OK(uv_random(uv_default_loop(),
                      &random_req,
                      &rdata,
                      1,
                      0,
                      fs_random_cb));
  ASSERT_OK(uv_getaddrinfo(uv_default_loop(),
                           &addrinfo_req,
                           fs_addrinfo_cb,
                           "example.invalid",
                           NULL,
                           NULL));

  /* Sleep for a moment to hopefully force the events to complete before
   * entering the event loop. */
  uv_sleep(100); //unit:msec

  ASSERT_OK(uv_run(uv_default_loop(), UV_RUN_DEFAULT));

  ASSERT_OK(uv_metrics_info(uv_default_loop(), &metrics));
  /* It's possible for uv__work_done() to execute one extra time even though the
   * QUEUE has already been cleared out. This has to do with the way we use an
   * uv_async to tell the event loop thread to process the worker pool QUEUE. */
  uint64_t loop_count;
  uint64_t events;
  uint64_t events_waiting;
  printf("loop_count:%d events:%d events_waiting:%d\r\n", metrics.loop_count, 
        metrics.events, metrics.events_waiting);
  ASSERT_GE(metrics.events, 7);
  /* It's possible one of the other events also got stuck in the event queue, so
   * check GE instead of EQ. Reason for 4 instead of 5 is because the call to
   * uv_getaddrinfo() is racey and slow. So can't guarantee that it'll always
   * execute before sleep completes. */
  ASSERT_GE(metrics.events_waiting, 4);
  ASSERT_EQ(pool_events_counter, -42);

  //在 loop 外进行操作必须要 cleanup_req ??
  uv_fs_unlink(NULL, &unlink_req, "test_file", NULL);
  uv_fs_req_cleanup(&unlink_req);

  MAKE_VALGRIND_HAPPY(uv_default_loop());
  return 0;
}
