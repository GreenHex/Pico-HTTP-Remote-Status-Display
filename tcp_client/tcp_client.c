/**
 * tcp_client.c
 * Copyright (c) 2025 MVK
 * 14-May-2025
 *
 */

#include "tcp_client.h"

#include "../not_resolved_list/not_resolved_list.h"
#include "../src/LCD_Test.h"
#include "../lib/LCD/LCD_1in3.h"
#include "../tiny-json/json_proc.h"
#include "../ble/ble.h"

extern uint total_requests_sent;
extern uint total_resolved_requests;
extern uint continuous_unresolved_request_count;
extern const uint UNRESOLVED_REQUEST_COUNT_THRESHOLD;
extern uint total_lwip_resets;

extern char myBuff[];

extern UWORD *BlackImage;

char header[22 + 23 + 18] = "\0";
const char header1[] = "GET / HTTP/1.1\r\nHOST: ";		// 22 chars + 16 chars
const char header2[] = "\r\nConnection: close\r\n\r\n"; // 23 chars

err_t recv_callback(void *arg,
					struct tcp_pcb *pcb,
					struct pbuf *p,
					err_t err)

{
	if ((err == ERR_OK) && (p != NULL))
	{
		printf(PRINT_GREEN "\nrecv_callback(): Processing request Id: %d\n" PRINT_RESET,
			   *(int *)arg);

		printf(PRINT_GREEN "               : Received total: %d, this buffer: %d, next: %d\n" PRINT_RESET,
			   p->tot_len,
			   p->len,
			   p->next);

		printf("\n%s\n", p->payload);

		printf(PRINT_YELLOW "=================================\n");
		printf("body: '{' is at %d, '}' is at %d\n", strcspn(p->payload, "{"), strcspn(p->payload, "}"));
		// THE HACK!!! Won't work with nested JSON
		strncpy(myBuff, p->payload + strcspn(p->payload, "{"), strcspn(p->payload, "}") - strcspn(p->payload, "{") + 1);
		myBuff[strcspn(p->payload, "}") - strcspn(p->payload, "{") + 1] = 0;
		printf("%s", myBuff);
		printf("\n=================================\n" PRINT_RESET);

		display_LCD(myBuff);

		tcp_recved(pcb, p->tot_len);

		if ((err = pbuf_free(p)) != ERR_OK)
		{
			printf(PRINT_RED "\npbuf_free() Err [%d]: %s\n" PRINT_RESET,
				   err,
				   lwip_strerr(-err));
		}

		if (remove_from_list(*(int *)arg))
		{
			++total_resolved_requests;
			continuous_unresolved_request_count = 0;

			printf(PRINT_YELLOW "\nProcessed request Id: %d\n" PRINT_RESET, *(int *)arg);
			printf(PRINT_RESET "\nRequests sent: %d\n", total_requests_sent);
			printf("Resolved requests: %d (%.2f%%)\n",
				   total_resolved_requests,
				   (float)total_resolved_requests * 100 / total_requests_sent);
			// printf("Unresolved requests: %d [%d]\n", total_requests_sent - total_resolved_requests, continuous_unresolved_request_count);
			printf("Unresolved requests: %d (%.2f%%)\n" PRINT_RESET,
				   total_requests_sent - total_resolved_requests,
				   (float)(total_requests_sent - total_resolved_requests) * 100 / total_requests_sent);
			// printf("lwip resets: %d\n" PRINT_RESET, total_lwip_resets);
			printf(PRINT_CYAN);
			print_list();
			printf(PRINT_RESET);
		}
		else
		{
			printf(PRINT_RED "\nAlready processed request Id: %d\n" PRINT_RESET, *(int *)arg);
		}
	}

	if (err = tcp_close(pcb))
	{
		printf(PRINT_RED "\ntcp_close() Err [%d]: %s\n" PRINT_RESET,
			   err,
			   lwip_strerr(-err));

		tcp_abort(pcb);
		return ERR_ABRT; // ATTENTION: When calling this from one of the TCP callbacks, make sure you always return ERR_ABRT
						 // (and never return ERR_ABRT otherwise or you will risk accessing deallocated memory or memory leaks!
	}

	return err;
}

err_t sent_callback(void *arg,
					struct tcp_pcb *pcb,
					u16_t len)
{
	printf(PRINT_CYAN "\nsent_callback(): Request Id: %d, Chars written: %d\n" PRINT_RESET,
		   *(int *)arg,
		   len);

	return ERR_OK;
}

err_t connected_callback(void *arg,
						 struct tcp_pcb *pcb,
						 err_t err)
{
	err = tcp_write(pcb, header, strlen(header), 0);

	err |= tcp_output(pcb);

	printf(PRINT_YELLOW "\nconnected_callback(): Request Id: %d, Err [%d]: %s\n" PRINT_RESET,
		   *(int *)arg,
		   err,
		   lwip_strerr(err));

	return err;
}

static void error_callback(void *arg, err_t err)
{
	printf(PRINT_RED "\nError: Processing request Id: %d, Err [%d]: %s\n" PRINT_RESET,
		   *(int *)arg,
		   err,
		   lwip_strerr(-err));

	return;
}

int http_req_tcp_send(char *ip_address,
					  u16_t port)
{
	static int i = 0;
	ip_addr_t ip = IPADDR4_INIT(IPADDR_ANY);
	// IP_ADDR4(&ip, 192, 168, 2, 11);

	strncpy(header, header1, strlen(header1));
	header[strlen(header1)] = 0;

	strncat(header, ip_address, strlen(ip_address));
	header[strlen(header1) + strlen(ip_address)] = 0;

	strncat(header, header2, strlen(header2));
	header[strlen(header1) + strlen(ip_address) + strlen(header2)] = 0;

	printf("\nReq header:\n%s", header);

	ip.addr = ipaddr_addr(ip_address);

	cyw43_arch_lwip_begin();

	struct tcp_pcb *pcb = tcp_new();

	tcp_sent(pcb, sent_callback);
	tcp_recv(pcb, recv_callback);
	tcp_arg(pcb, &i);
	tcp_err(pcb, error_callback);

	err_t err = tcp_connect(pcb,
							&ip,
							port,
							connected_callback);

	cyw43_arch_lwip_end();

	add_to_list(++i);

	return ERR_OK;
}
