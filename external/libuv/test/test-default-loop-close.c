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


static int timer_cb_called;


static void timer_cb(uv_timer_t* timer) {
  timer_cb_called++;
  printf("%s data:\n", __func__, timer->data);
  // uv_timer_t 类型是 uv_handle_t 的子类
  // uv_close((uv_handle_t*) timer, NULL);
}


TEST_IMPL(default_loop_close) {
  uv_loop_t* loop;
  uv_timer_t timer_handle;
  int ret;

  loop = uv_default_loop();
  ASSERT_NOT_NULL(loop);

  timer_handle.data = (void*)malloc(256);
  ASSERT_NOT_NULL(timer_handle.data);

  // TODO:这里在 uv_timer_t::data 域中添加的数据, 为什么在 timer_cb 中为 null
  strcpy(timer_handle.data, "Timer1");
  ASSERT_OK(uv_timer_init(loop, &timer_handle));
  ASSERT_OK(uv_timer_start(&timer_handle, timer_cb, 1000, 0));
  ASSERT_OK(uv_run(loop, UV_RUN_DEFAULT));
  ASSERT_EQ(1, timer_cb_called);
  ret = uv_loop_close(loop);
  ASSERT_EQ(ret, UV_EBUSY); 
  //ASSERT_OK(uv_loop_close(loop));

  loop = uv_default_loop(); // default_loop 被关闭后, 仍然可以继续使用
  ASSERT_NOT_NULL(loop);

  strcpy(timer_handle.data, "Timer2");
  ASSERT_OK(uv_timer_init(loop, &timer_handle));
  ASSERT_OK(uv_timer_start(&timer_handle, timer_cb, 1000, 1000));
  ASSERT_OK(uv_run(loop, UV_RUN_DEFAULT));
  ASSERT_EQ(2, timer_cb_called);

  free(timer_handle.data);

  MAKE_VALGRIND_HAPPY(loop);
  return 0;
}
