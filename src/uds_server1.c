/**
 * @brief UDS client-server test
 * 
*/
#include <stdio.h>
#include <stdlib.h>
#include "uv.h"


#define UDS_SERVER_PATH "/tmp/libuv_server.sock"
#define UDS_CLIENT_PATH "/tmp/unix_client.sock"


uv_loop_t *get_loop(void);


typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;


static     uv_pipe_t s_server;

// libuv 的进程与 shell 创建的标准进程通过 UDS 通信
// libuv 创建的 server


void free_write_req(uv_write_t *req)
{
    printf("%s\n", __func__);
    write_req_t *wr = (write_req_t*) req;
    free(wr->buf.base);
    free(wr);
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    printf("%s\n", __func__);
    buf->base = malloc(suggested_size);
    buf->len = suggested_size;
}

void echo_write(uv_write_t *req, int status)
{
    printf("%s\n", __func__);
    if (status < 0) {
        fprintf(stderr, "Write error %s\n", uv_err_name(status));
    }
    free_write_req(req);
}

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
    printf("%s\n", __func__);
    if (nread > 0) {
        write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
        req->buf = uv_buf_init(buf->base, nread);
        uv_write((uv_write_t*) req, client, &req->buf, 1, echo_write);
        return;
    }

    if (nread < 0) {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t*) client, NULL);
    }

    free(buf->base);
}


static void on_new_connection(uv_stream_t *server, int status)
{
    int ret;
    printf("%s\n", __func__);
    if (status == -1) {
        printf("connection error\n");
        return;
    }

    uv_pipe_t *client = (uv_pipe_t*)malloc(sizeof(uv_pipe_t));
    uv_pipe_init(get_loop(), client, 0);
    ret = uv_accept(server, (uv_stream_t*)client);
    if (ret == 0) {
        printf("client connect success\n");
        uv_read_start((uv_stream_t*)client, alloc_buffer, echo_read);
    } else {
        uv_close((uv_handle_t*)client, NULL);
    }
}


/**
 * @brief 
 * 
*/
int start_server1(uv_loop_t *loop)
{
    printf("server[%ld] start\n");
    int ret;

    uv_pipe_init(loop, &s_server, 0);
    printf("1\n");
    unlink(UDS_SERVER_PATH);

    printf("2\n");
    ret = uv_pipe_bind(&s_server, UDS_SERVER_PATH);
    if (ret != 0) {
        fprintf(stderr, "Bind error %s\n", uv_err_name(ret));
        return -1;
    }

    printf("3\n");
    ret = uv_listen((uv_stream_t*)&s_server, 2, on_new_connection);
    if (ret != 0) {
        fprintf(stderr, "Listen error %s\n", uv_err_name(ret));
        return -2;
    }
    printf("4\n");
}