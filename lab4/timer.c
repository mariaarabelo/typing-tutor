#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"


int timer_hook_id =0;
int counter =0;
int (timer_subscribe_int)(uint8_t *bit_no) {
 
  if (bit_no==NULL) return 1;
  *bit_no = BIT(timer_hook_id); 

  // o que se faz com o bit_no?
  // hook id tem de ser entre 0 e 7? pq?
  // depois da syscall, o bit_no vai mudar ?
  // bit_no vai ser sempre zero?

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer_hook_id) != OK)
    return 1;
  // it enables the corresponding interrupt
  // (so we dont need to call sys_irqenable())
  // IRQ_REENABLE (int, the policy) so that the generic
  // interrupt handler will acknowledge the interrupt,
  // output the EOI command to the PIC
  // enabling further interrupts on the corresponding IRQ line
  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&timer_hook_id)!=OK)
    return 1; 
  return 0;
}

void (timer_int_handler)() {
  // increments a global counter variable
  counter++;
}
