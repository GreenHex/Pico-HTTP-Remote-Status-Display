/**
 * http_client.c
 * Copyright (c) 2025 MVK
 * 13-May-2025
 *
 */

#include "http_client.h"
#include "../src/LCD_Test.h"
#include "../lib/LCD/LCD_1in3.h"
#include "../ble/ble.h"
#include "../tiny-json/json_proc.h"

#include <strings.h>
#include <string.h>

const absolute_time_t REQUEST_DELAY = 30000000; // arbitrary pico time number

// #define HTTPC_POLL_TIMEOUT 30 // seconds // 15 // 20000

// HTTP Client
extern char myBuff[];

extern uint total_requests_sent;
extern uint total_resolved_requests;
extern uint continuous_unresolved_request_count;
extern const uint UNRESOLVED_REQUEST_COUNT_THRESHOLD;
extern uint total_lwip_resets;

void result_fn(void *arg, httpc_result_t httpc_result,
			   u32_t rx_content_len,
			   u32_t srv_res, err_t err)
{
	printf(PRINT_CYAN "\n>>> result_fn >>>\n");
	printf("httpc_result: %s\n",
		   httpc_result == HTTPC_RESULT_OK				  ? "HTTPC_RESULT_OK"
		   : httpc_result == HTTPC_RESULT_ERR_UNKNOWN	  ? "HTTPC_RESULT_ERR_UNKNOWN"
		   : httpc_result == HTTPC_RESULT_ERR_CONNECT	  ? "HTTPC_RESULT_ERR_CONNECT"
		   : httpc_result == HTTPC_RESULT_ERR_HOSTNAME	  ? "HTTPC_RESULT_ERR_HOSTNAME"
		   : httpc_result == HTTPC_RESULT_ERR_CLOSED	  ? "HTTPC_RESULT_ERR_CLOSED"
		   : httpc_result == HTTPC_RESULT_ERR_TIMEOUT	  ? "HTTPC_RESULT_ERR_TIMEOUT"
		   : httpc_result == HTTPC_RESULT_ERR_SVR_RESP	  ? "HTTPC_RESULT_ERR_SVR_RESP"
		   : httpc_result == HTTPC_RESULT_ERR_MEM		  ? "HTTPC_RESULT_ERR_MEM"
		   : httpc_result == HTTPC_RESULT_LOCAL_ABORT	  ? "HTTPC_RESULT_LOCAL_ABORT"
		   : httpc_result == HTTPC_RESULT_ERR_CONTENT_LEN ? "HTTPC_RESULT_ERR_CONTENT_LEN"
														  : "*UNKNOWN*");

	if (httpc_result == HTTPC_RESULT_OK)
	{
		++total_resolved_requests;
		continuous_unresolved_request_count = 0;
	}
	printf("received %ld bytes\n", rx_content_len);
	printf("server response: %ld\n", srv_res);
	printf("err: %d\n", err);
	printf("<<< result_fn <<<\n" PRINT_RESET);

	return;
}

err_t headers_done_fn(httpc_state_t *connection,
					  void *arg,
					  struct pbuf *hdr,
					  u16_t hdr_len,
					  u32_t content_len)
{
	printf(PRINT_YELLOW "\n>>> headers_done_fn >>>\n");
	printf("content length=%d\n", content_len);
	printf("header length %d\n", hdr_len);
	pbuf_copy_partial(hdr, myBuff, hdr->tot_len, 0);
	printf(PRINT_RED ">>>HEADERS>>>\n" PRINT_YELLOW);
	printf("%s", myBuff);
	printf("<<< headers_done_fn <<<\n" PRINT_RESET);

	return ERR_OK;
}

err_t recv_fn(void *arg,
			  struct tcp_pcb *tpcb,
			  struct pbuf *p,
			  err_t err)
{
	printf(PRINT_GREEN "\n>>> recv_fn >>>\n");
	if (p == NULL)
	{
		printf("p is NULL\n");
	}
	else
	{
		printf("p: %p\n", p);
		printf("next: %p\n", p->next);
		printf("payload: %p\n", p->payload);
		printf("tot_len: %d\n", p->tot_len);
		printf("len: %d\n", p->len);
		printf("\n=================================\n");
		printf("body: '{' is at %d, '}' is at %d\n", strcspn(p->payload, "{"), strcspn(p->payload, "}"));
		// THE HACK!!! Won't work with nested JSON
		strncpy(myBuff, p->payload + strcspn(p->payload, "{"), strcspn(p->payload, "}") - strcspn(p->payload, "{") + 1);
		myBuff[strcspn(p->payload, "}") - strcspn(p->payload, "{") + 1] = 0;
		printf("%s", myBuff);
		printf("\n=================================\n");

		printf(PRINT_RESET "\nRequests sent: %d\n", total_requests_sent);
		printf("Resolved requests: %d\n", total_resolved_requests);
		printf("Unresolved requests: %d [%d]\n", total_requests_sent - total_resolved_requests, continuous_unresolved_request_count);
		printf("lwip resets: %d\n" PRINT_RESET, total_lwip_resets);

		display_LCD(myBuff);
	}
	printf("<<< recv_fn <<<\n" PRINT_RESET);

	return ERR_OK;
}
