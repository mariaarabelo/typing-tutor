#include <lcom/lcf.h>
#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"

extern int timer_hook_id;
extern int keyboard_hook_id;
extern uint32_t sys_counter;
extern uint8_t data; // scancode
extern int counter;

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

// Handle interrupts from more than one I/O device

//Tests reading of scancodes via KBD interrupts.
//Displays the scancodes received from the keyboard using interrupts.
//Exits upon release of the ESC key

// first byte of 2-bytes scancodes use to be 0xE0

int(kbd_test_scan)(){
    // subscribe the KBC interrupts
	uint8_t irq_set;
    int ipc_status;
    message msg;
    bool is_2bytes= false;
    uint8_t temp;
    if (keyboard_subscribe_int(&irq_set) != OK) return 1;
    // data is an extern variable
    while (data != ESC){ // 0x81 : breakcode of ESC
        if (driver_receive(ANY, &msg, &ipc_status)!=0){
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
                        kbc_ih(); // read the scancode from OUT_BUF
                        // read only one byte per interrupt
                        // print the scancodes (makecode and breakcode)    
                        if (data == 0xE0) {
                            is_2bytes = true;
                            temp = data;
                            break; 
                        } else {
                            if (is_2bytes) {
                                is_2bytes = false;
                                uint8_t final[2] = {temp, data};
                                kbd_print_scancode(!(data & BREAK), 2, final);
                            } else {
                                kbd_print_scancode(!(data & BREAK), 1, &data);
                            }
                        }
                        // make Whether this is a make or a break code
                        // size Size in bytes of the scancode
                        // bytes Array with size elements, with the scancode bytes
                    }
                    break;
                default:
                    break;
            }
        }
    }
    if (keyboard_unsubscribe_int() != OK) return 1;
    if (kbd_print_no_sysinb(sys_counter)!= OK) return 1; // uint32_t 
    return 0;
}

int(kbd_test_poll)(){
    bool is_2bytes= false;

    while (data != ESC) {
        if (kbc_polling() == OK){
            if (data == 0xE0) {
                is_2bytes = true;
                continue;
            } else {
                if (is_2bytes) {
                    is_2bytes = false;
                    uint8_t final[2] = {0xE0, data};
                    kbd_print_scancode(!(data & BREAK), 2, final);
                } else {
                    kbd_print_scancode(!(data & BREAK), 1, &data);
                }
            }
        }
    }
    if (kbc_activate_interrupts() != OK) return 1;
    if (kbd_print_no_sysinb(sys_counter)!= OK) return 1; // uint32_t 
    return 0;
}

int(kbd_test_timed_scan)(uint8_t idle){
    printf("this program will wait at most %d seconds for some scancode\n", idle);
    // subscribe the KBC interrupts
	uint8_t kbd_irq_set; //interrupt request line
    uint8_t timer_irq_set;
    int ipc_status;
    message msg;
    bool is_2bytes= false;
    uint8_t temp;

    if (keyboard_subscribe_int(&kbd_irq_set) != OK) return 1;
    if (timer_subscribe_int(&timer_irq_set) != OK) return 1;
    
    int r;
    while (counter < idle*60 && data != ESC) {
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
                    if (msg.m_notify.interrupts & kbd_irq_set){ // subscribed interrupt
                        kbc_ih(); // read the scancode from OUT_BUF
                        // read only one byte per interrupt
                        // print the scancodes (makecode and breakcode)    
                        if (data == 0xE0) {
                            is_2bytes = true;
                            temp = data;
                            break;  
                        } else {
                            if (is_2bytes) {
                                is_2bytes = false;
                                uint8_t final[2] = {temp, data};
                                kbd_print_scancode(!(data & BREAK), 2, final);
                            } else {
                                kbd_print_scancode(!(data & BREAK), 1, &data);
                            }
                        }
                        counter = 0;
                    }
                    if (msg.m_notify.interrupts & timer_irq_set) {
                        timer_int_handler();
                        if (counter%60 ==0){
                            timer_print_elapsed_time();
                            printf("secs = %d\n", counter/60);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        
    }

    if (keyboard_unsubscribe_int() != OK) return 1;
    if (timer_unsubscribe_int() != OK) return 1;

    if (kbd_print_no_sysinb(sys_counter)!= OK) return 1; // uint32_t 
    printf("counter = %d\n", counter);
    return 0;
}
