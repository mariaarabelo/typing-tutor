// LCOM specific
#include <lcom/lab5.h>
#include <lcom/lcf.h>

#include "uart.h"

#include <stdint.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab7/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab7/output.txt");
  if (lcf_start(argc, argv))
    return 1;
  lcf_cleanup();
  return 0;
}

/* 
Adapting lab5 to test
 */

int(video_test_controller)() {
  /* Read config */
  struct uart_config_t uart_config;
  bzero(&uart_config, sizeof uart_config);
  read_uart_config(&uart_config);
  printf("UART CONFIG\n");
  printf("bits per char: %d\n", uart_config.no_bits_per_char);
  printf("parity: %d\n", uart_config.parity);
  printf("stop bits: %d\n", uart_config.no_stop_bits);
  printf("bitrate: %lu\n", uart_config.bitrate);

  /*
   Read bytes until user stops the program 
  */
  
  int bit_no = 0;
  uart_subscribe_int(&bit_no);
  int ipc_status;
  message msg;
  int r;
  uint32_t irq_set = BIT(bit_no);
  while (true) {
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & irq_set) {
          uart_ih();
        }
      }
    }
  }
  uart_unsubscribe_int();

  return 0;
}
