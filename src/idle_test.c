// static void timer_cb2(uv_timer_t* handle) {
// //   ASSERT_PTR_EQ(handle, &timer_handle);
//     timer_called = 1;
//     printf("timer_cb2\n");
// }

// static void poll_cb_fail(uv_fs_poll_t* handle,
//                          int status,
//                          const uv_stat_t* prev,
//                          const uv_stat_t* curr) {
// //   ASSERT(0 && "fail_cb called");
//     printf("fail_cb called status:%d\n", status);
// }

// static void idle_cb(uv_idle_t *handle)
// {
//     ++idle_cnt;
//     printf("idle:%d\n", idle_cnt);
//     if (idle_cnt > 5) {
//         printf("stop idle\n");
//         uv_idle_stop(handle);
//     }
// }