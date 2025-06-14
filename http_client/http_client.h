/**
 * http_client.h
 * Copyright (c) 2025 MVK
 * 13-May-2025
 *
 */

#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include "lwip/apps/http_client.h"

extern const absolute_time_t REQUEST_DELAY;

// #define HTTPC_POLL_TIMEOUT 30 // seconds // 15 // 20000

extern void result_fn(void *arg, httpc_result_t httpc_result,
					  u32_t rx_content_len,
					  u32_t srv_res, err_t err);

extern err_t headers_done_fn(httpc_state_t *connection,
							 void *arg,
							 struct pbuf *hdr,
							 u16_t hdr_len,
							 u32_t content_len);

extern err_t recv_fn(void *arg,
					 struct tcp_pcb *tpcb,
					 struct pbuf *p,
					 err_t err);

#endif // _HTTP_CLIENT_H_
