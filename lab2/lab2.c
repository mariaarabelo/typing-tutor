#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

// make
// lcom_run
// lcom_run lab2 "config 0 all -t 0"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}
 // only using timer 0 
 // do not change timer 1 configuration
int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  // timer 0, 1, 2
  // tsf_all - status byte, in hexadecimal
  // tsf_initial - initialization mode
  // tsf_mode - counting mode
  // tsf_base - counting base

  uint8_t st; // status
  
  if (timer_get_conf(timer, &st) != OK) {
    printf("Error in timer_get_conf\n");
    return 1;
  }  // lê a configuração do timer
  
  if (timer_display_conf(timer, st, field) != OK) {
    printf("Error in timer_display_conf\n");
    return 1;
  }  // mostra a configuração do timer

  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  if (freq < 19 || timer > 2) return 1; 
  // freq cant be <19 so it doesnt overflow

  if (timer_set_frequency(timer, freq) !=0) return 1; 
  return 1;
}

extern int counter;
int(timer_test_int)(uint8_t time) {

  uint8_t irq_set =0; // nao precisava ser inicializada...
  if (timer_subscribe_int(&irq_set) != OK) return 1;

  int ipc_status;
  message msg;
  int r;

  while (time>0){
    if ((r=driver_receive(ANY, &msg, &ipc_status))!=0){
      // msg and ipc_status will be initialized by driver_receive()
      printf("driver_receive failed with: %d, r");
      continue;
    }
    if (is_ipc_notify(ipc_status)){ // received notification
      switch (_ENDPOINT_P(msg.m_source)) 
      // ENDPOINT_P extracts the process identifier from a process's endpoint
      {
      case HARDWARE:
        if (msg.m_notify.interrupts & irq_set){ // subscribed interrupt
          timer_int_handler();
          if (counter%60 ==0){
            timer_print_elapsed_time();
            time--;
          }
        }
        break;
      default:
        break;
      }
    }
  }
  
  if (timer_unsubscribe_int() != OK) return 1;

  return 0;
}
