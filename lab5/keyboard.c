#include <lcom/lcf.h>

#include <stdint.h>
#include "keyboard.h"

int keyboard_hook_id =1;
uint8_t data;

int (keyboard_subscribe_int)(uint8_t *bit_no) {
 
  if (bit_no==NULL) return 1;
  *bit_no = BIT(keyboard_hook_id); 

  if (sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &keyboard_hook_id) != OK)
    return 1;
  // it enables the corresponding interrupt (so we dont need to call sys_irqenable())
  // IRQ_REENABLE (int, the policy) so that the generic
  // interrupt handler will acknowledge the interrupt,
  // output the EOI command to the PIC
  // enabling further interrupts on the corresponding IRQ line
  return 0;
}

int (keyboard_unsubscribe_int)() {
  return sys_irqrmpolicy(&keyboard_hook_id);
}

void (kbc_ih)(){
  //read the status register and check if there was some communications error;
  uint8_t st;
  if (util_sys_inb(0x64 , &st) != OK) // lê o status register
  {
    printf("reading status register 0x64 was not ok\n");  
    return;
  } 

  if (st & (BIT(7))) {
    printf("Parity Error.\n");
    return;
  }
  
  if (st & (BIT(6))) {
    printf("Timeout Error.\n");
    return;
  }

  if ((st & BIT(0)) != 0){ // if the output buffer is full, we can read the scancode 
    //read the scancode byte from the output buffer;
    if (util_sys_inb(0x60 , &data) != OK){
      printf("reading data from output buffer 0x60 was not ok");  
      return;
    }
    return; // success
  }      
}

int (wait_ESC)(){
uint8_t irq_set;
  int ipc_status, r;
  message msg;

  if (keyboard_subscribe_int(&irq_set)) return 1;
  printf("Subscribed keyboard interrupts\n");

  while (data != ESC){ // 0x81 : breakcode of ESC
      if ((r=driver_receive(ANY, &msg, &ipc_status))!=0){
          printf("Driver_receive failed %d \n", r);
          continue;
      }
      if (is_ipc_notify(ipc_status)){ // received notification
          switch (_ENDPOINT_P(msg.m_source)) 
          // ENDPOINT_P extracts the process identifier from a process's endpoint
          {
              case HARDWARE:
                  if (msg.m_notify.interrupts & irq_set){ 
                      printf("Interrupt received\n");
                      kbc_ih(); // read the scancode from OUT_BUF
                      // read only one byte per interrupt 
                  }
                  break;
              default:
                  break;
          }
      }
  }
  printf("ESC released, 0x81\n");
  return keyboard_unsubscribe_int();
}

