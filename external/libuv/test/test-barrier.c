/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
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

#include <string.h>
#include <errno.h>

typedef struct {
  uv_barrier_t barrier;
  unsigned delay;
  unsigned niter;
  unsigned main_barrier_wait_rval;
  unsigned worker_barrier_wait_rval;
} worker_config;


static void worker(void* arg) {
  worker_config* cfg = arg;
  unsigned i;
  printf("%s enter\n", __func__);

  if (cfg->delay)
    uv_sleep(cfg->delay);

  for (i = 0; i < cfg->niter; i++)
    cfg->worker_barrier_wait_rval += uv_barrier_wait(&cfg->barrier);
  printf("%s exit\n", __func__);
}


TEST_IMPL(barrier_1) {
  uv_thread_t thread;
  worker_config wc;
  printf("main enter\n");

  memset(&wc, 0, sizeof(wc));
  wc.niter = 1;
  // wc.delay = 3000;

  ASSERT_OK(uv_barrier_init(&wc.barrier, 2)); // 一个子线程 + 1 个主线程
  ASSERT_OK(uv_thread_create(&thread, worker, &wc));

  uv_sleep(1000);
  wc.main_barrier_wait_rval = uv_barrier_wait(&wc.barrier);
  // 主线程和子线程同时阻塞等待

  ASSERT_OK(uv_thread_join(&thread));
  uv_barrier_destroy(&wc.barrier);

  ASSERT_EQ(1, (wc.main_barrier_wait_rval ^ wc.worker_barrier_wait_rval));
  printf("main exit\n");

  return 0;
}


TEST_IMPL(barrier_2) {
  uv_thread_t thread;
  worker_config wc;

  memset(&wc, 0, sizeof(wc));
  wc.delay = 2000;
  wc.niter = 1;

  ASSERT_OK(uv_barrier_init(&wc.barrier, 2));
  ASSERT_OK(uv_thread_create(&thread, worker, &wc));

  wc.main_barrier_wait_rval = uv_barrier_wait(&wc.barrier);

  ASSERT_OK(uv_thread_join(&thread));
  uv_barrier_destroy(&wc.barrier);

  ASSERT_EQ(1, (wc.main_barrier_wait_rval ^ wc.worker_barrier_wait_rval));

  return 0;
}


TEST_IMPL(barrier_3) {
  uv_thread_t thread;
  worker_config wc;
  unsigned i;

  memset(&wc, 0, sizeof(wc));
  wc.niter = 5;

  ASSERT_OK(uv_barrier_init(&wc.barrier, 2));
  ASSERT_OK(uv_thread_create(&thread, worker, &wc));

  for (i = 0; i < wc.niter; i++)
    wc.main_barrier_wait_rval += uv_barrier_wait(&wc.barrier);

  ASSERT_OK(uv_thread_join(&thread));
  uv_barrier_destroy(&wc.barrier);

  ASSERT_EQ(wc.niter, wc.main_barrier_wait_rval + wc.worker_barrier_wait_rval);

  return 0;
}

static void serial_worker(void* data) {
  uv_barrier_t* barrier;
  unsigned i;

  barrier = data;
  for (i = 0; i < 5; i++) {
    uv_barrier_wait(barrier);
    printf("child barrier[%d] wait\n", i);
  }
  if (uv_barrier_wait(barrier) > 0)
    uv_barrier_destroy(barrier);

  uv_sleep(100);  /* Wait a bit before terminating. */
  printf("child exit\n");
}

/* Ensure that uv_barrier_wait returns positive only after all threads have
 * exited the barrier. If this value is returned too early and the barrier is
 * destroyed prematurely, then this test may see a crash. */
TEST_IMPL(barrier_serial_thread) {
  uv_thread_t threads[4];
  uv_barrier_t barrier;
  unsigned i;

  printf("main enter\n");
  ASSERT_OK(uv_barrier_init(&barrier, ARRAY_SIZE(threads) + 1));

  for (i = 0; i < ARRAY_SIZE(threads); ++i)
    ASSERT_OK(uv_thread_create(&threads[i], serial_worker, &barrier));

  for (i = 0; i < 5; i++) {
    uv_barrier_wait(&barrier);
    printf("main barrier[%d] wait\n", i);
  }
  if (uv_barrier_wait(&barrier) > 0)
    uv_barrier_destroy(&barrier);

  for (i = 0; i < ARRAY_SIZE(threads); ++i)
    ASSERT_OK(uv_thread_join(&threads[i]));
  printf("main exit\n");

  return 0;
}

/* Single thread uv_barrier_wait should return correct return value. */
TEST_IMPL(barrier_serial_thread_single) {
  uv_barrier_t barrier;

  ASSERT_OK(uv_barrier_init(&barrier, 1));
  ASSERT_LT(0, uv_barrier_wait(&barrier));
  uv_barrier_destroy(&barrier);
  return 0;
}
