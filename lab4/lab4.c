#include <lcom/lcf.h>
#include <lcom/lab4.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "mouse.h"
#include "timer.c"

extern int timer_hook_id;
extern int mouse_hook_id;
extern int counter;
extern struct packet pp;
extern int bytes_read;
extern enum state state;

// doxygen 
// https://web.fe.up.pt/~pfs/aulas/lcom2324/labs/lab4/src/doc/files.html

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (mouse_test_packet)(uint32_t cnt) {
    uint8_t mouse_irq_set;
    int ipc_status;
    message msg;
    // if (mouse_enable_data_reporting() != OK) return 1; // done with 0xF4 command
    if (mouse_command(0xF4) != OK) return 1;
    
    if (mouse_subscribe_int(&mouse_irq_set) != OK) return 1;
    int r;
    uint32_t packets_read = 0;
    while (packets_read < cnt) {
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
                    if (msg.m_notify.interrupts & mouse_irq_set){ // subscribed interrupt
                        mouse_ih(); // reads only one byte per interrupt
                        sync();
                        if (bytes_read ==3){
                            parse();
                            mouse_print_packet(&pp);
                            packets_read++;
                            bytes_read = 0;
                        }              
                    }
                    break;
                default:
                    break;
            }
        }
    }
    if (mouse_unsubscribe_int() != OK) return 1;
    if (mouse_command(0xF5) != OK) return 1;
    
    return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
    // mouse_test_packet() + kbd_test_timed_scan()
    uint8_t mouse_irq_set;
    uint8_t timer_irq_set;
    int ipc_status;
    message msg;
    if (mouse_command(0xF4) != OK) return 1;    
    printf("data reporting enabled! \n");
    if (mouse_subscribe_int(&mouse_irq_set) != OK) return 1;
    if (timer_subscribe_int(&timer_irq_set) != OK) return 1;
    int r;
    uint32_t packets_read = 0;
    while (counter < idle_time*60) {
        // msg and ipc_status will be initialized by driver_receive()
        if ((r=driver_receive(ANY, &msg, &ipc_status))!=0){
            printf("driver_receive failed with: %d, r");
            continue;
        }
        if (is_ipc_notify(ipc_status)){ // received notification
            switch (_ENDPOINT_P(msg.m_source)) 
            // ENDPOINT_P extracts the process identifier from a process's endpoint
            {
                case HARDWARE:
                    if (msg.m_notify.interrupts & mouse_irq_set){ // subscribed interrupt
                        mouse_ih(); // reads only one byte per interrupt
                        sync();
                        if (bytes_read ==3){
                            parse();
                            mouse_print_packet(&pp);
                            packets_read++;
                            bytes_read = 0;
                            counter = 0;
                        }              
                    }
                    if (msg.m_notify.interrupts & timer_irq_set) {
                        timer_int_handler();  // counter++
                        if (counter%60 ==0){
                            timer_print_elapsed_time();
                            printf("secs = %d\n", counter/60);
                        }
                        // determine timer 0 freq with sys_hz()
                    }
                    break;
                default:
                    break;
            }
        }
    }
    if (mouse_unsubscribe_int() != OK) return 1;
    if (timer_unsubscribe_int() != OK) return 1;
    if (mouse_command(0xF5) != OK) return 1;
    
    return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance){
    uint8_t mouse_irq_set;
    printf("will enable data reporting\n "); 
    if (mouse_command(0xF4) != OK) return 1;
    printf("data reporting enabled! \n");
    if (mouse_subscribe_int(&mouse_irq_set) != OK) return 1;
    int ipc_status;
    message msg;
    int r;
    uint32_t packets_read = 0;
    while (state != STATE_END) {
        printf("before process_packet the state is %d\n", state);    
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
                    if (msg.m_notify.interrupts & mouse_irq_set){ // subscribed interrupt
                        printf("received an interrupt\n");
                        mouse_ih(); // reads only one byte per interrupt
                        sync();
                        if (bytes_read ==3){
                            parse();
                            mouse_print_packet(&pp);
                            process_packet(&pp, x_len, tolerance);
                            packets_read++;
                            bytes_read = 0;
                        }              
                    }
                    break;
                default:
                    break;
            }
        }
    }
    if (mouse_unsubscribe_int() != OK) return 1;
    if (mouse_command(0xF5) != OK) return 1;
    return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
