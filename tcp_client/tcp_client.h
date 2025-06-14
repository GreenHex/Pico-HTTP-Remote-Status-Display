/**
 * tcp_client.h
 * Copyright (c) 2025 MVK
 * 14-May-2025
 *
 */

#ifndef _TCP_CLIENT_H_
#define _TCP_CLIENT_H_

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/ip_addr.h"
#include "lwip/tcp.h"

int http_req_tcp_send(char *ip_address, u16_t port);

#endif // _TCP_CLIENT_H_
