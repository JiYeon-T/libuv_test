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
#ifndef _WIN32
#include <unistd.h>
#endif

static unsigned int read_cb_called;

static void alloc_cb(uv_handle_t* handle, size_t size, uv_buf_t* buf) {
  printf("%s\n", __func__);
  static char slab[1];
  buf->base = slab;
  buf->len = sizeof(slab);
}

static void read_cb(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
  printf("%s read_cb_called:%d nread:%d\n", __func__, read_cb_called, nread);
  switch (++read_cb_called) {
  case 1:
    ASSERT_EQ(1, nread);
    uv_read_stop(handle);
    break;
  case 2:
    // uv_read_stop 导致异步 IO 返回,读取数据长度 UV_EOF
    ASSERT_EQ(nread, UV_EOF);
    uv_close((uv_handle_t *) handle, NULL);
    break;
  default:
    ASSERT(!"read_cb_called > 2");
  }
}

// TODO:
// C 语言通过指针实现的继承(类 & 子类)
/*
 * uv_pipe_t is a subclass of uv_stream_t.
 *
 * Representing a pipe stream or pipe server. On Windows this is a Named
 * Pipe. On Unix this is a Unix domain socket.
 */
// struct uv_pipe_s {
//   UV_HANDLE_FIELDS
//   UV_STREAM_FIELDS
//   int ipc; /* non-zero if this pipe is used for passing handles */
//   UV_PIPE_PRIVATE_FIELDS
// };

TEST_IMPL(close_fd) {
  uv_pipe_t pipe_handle;
  uv_fs_t req;
  uv_buf_t bufs[1];
  uv_file fd[2];
  bufs[0] = uv_buf_init("", 1);

  printf("main enter\n");
  ASSERT_OK(uv_pipe(fd, 0, 0));
  ASSERT_OK(uv_pipe_init(uv_default_loop(), &pipe_handle, 0));
  // TODO:
  // 打开管道的读端
  //管道还需要再打开读端吗? 返回的不就是 fd 吗
  ASSERT_OK(uv_pipe_open(&pipe_handle, fd[0]));
  /* uv_pipe_open() takes ownership of the file descriptor. */
  fd[0] = -1;

  //先向写端写数据
  // ASSERT_EQ(1, uv_fs_write(NULL, &req, fd[1], bufs, 1, -1, NULL));
  // ASSERT_EQ(1, req.result);
  // uv_fs_req_cleanup(&req);
#ifdef _WIN32
  ASSERT_OK(_close(fd[1]));
#else
  //关闭管道写端, 
  //关闭管道写端 也会导致 uv_read_start 返回, 但是返回数据长度为 
  ASSERT_OK(close(fd[1]));
#endif
  fd[1] = -1;
  ASSERT_OK(uv_read_start((uv_stream_t *) &pipe_handle, alloc_cb, read_cb));
  ASSERT_OK(uv_run(uv_default_loop(), UV_RUN_DEFAULT));
  ASSERT_EQ(1, read_cb_called);
  printf("main exit1\n");

  ASSERT_OK(uv_is_active((const uv_handle_t *) &pipe_handle));
  //Read data from an incoming stream. The uv_read_cb callback will be made 
  //several times until there is no more data to read or uv_read_stop() is called.
  ASSERT_OK(uv_read_start((uv_stream_t *) &pipe_handle, alloc_cb, read_cb));
  ASSERT_OK(uv_run(uv_default_loop(), UV_RUN_DEFAULT));
  ASSERT_EQ(2, read_cb_called);
  ASSERT_NE(0, uv_is_closing((const uv_handle_t *) &pipe_handle));
  printf("main exit2\n");

  MAKE_VALGRIND_HAPPY(uv_default_loop());
  return 0;
}
