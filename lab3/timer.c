#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int timer_hook_id =0;
int counter =0;

// configure the specified timer (one of 0, 1 and 2) 
// to generate a time base with a given frequency in Hz.
// timer 0 -> maintain time of the day, default 60Hz
// on every timer 0 interrupt, it increments a counter variable
int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  if (freq > TIMER_FREQ || freq < 19 || timer > 2) return 1; 
  
  // not to change the 4 LSbits (counting mode and BCD) of control word!
  //  read the old input timer configuration 
  uint8_t st;
  if (timer_get_conf(timer, &st) != 0) return 1;

  // change timer configuration
  uint8_t new_st;
  new_st = (st & 0x0F) | TIMER_LSB_MSB; 
  // 0x0F = 00001111
  
  switch (timer)
  {
  case 0:
    new_st |= TIMER_SEL0;
    break;
  case 1:
    new_st |= TIMER_SEL1;
    break;
  case 2:
    new_st |= TIMER_SEL2;
    break;
  default:
    return 1;
  }

  // dizer ao i8254 que vamos configurar o timer
  sys_outb(TIMER_CTRL, new_st);

  uint16_t counter_value = TIMER_FREQ / freq;
  //TIMER_FREQ 1193182 
  //clock frequency for timer in PC and AT */

  uint8_t lsb, msb;
  util_get_LSB(counter_value, &lsb);
  util_get_MSB(counter_value, &msb);

  // setar o counter_value no timer
  if (sys_outb(TIMER_0 + timer, lsb) != 0) return 1;
  if (sys_outb(TIMER_0 + timer, msb) != 0) return 1;

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
 
  if (bit_no==NULL) return 1;
  *bit_no = BIT(timer_hook_id); 

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

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  // Reads the input timer configuration (status) via read-back command.
  // st	- Address of memory position to be filled with the timer config

  if (st == NULL) return 1;  // Verificar se o ponteiro não é NULL
  if (timer < 0 || timer > 2) return 1;  // Verificar se o timer é válido

  uint8_t rb_command = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer); 
  // TIMER_RB_CMD - read-back command (BIT(7) | BIT(6))
  // TIMER_RB_COUNT_ - read-back count BIT(5), o count não é lido, apenas o status
  // TIMER_RB_SEL(timer) - select the timer to read BIT((n) + 1)

  if (sys_outb(TIMER_CTRL, rb_command) != OK) return 1;  // envia o RBC p/ o controlador
  // TIMER_CTRL 0x43 - Control register

  if (util_sys_inb(TIMER_0 + timer, st) != OK) return 1;  // lê o status do timer

  return 0;
}

/* UNION
- permite armazenar diferentes tipos de dados no mesmo local de memória
- pode conter vários membros (como structs), 
mas apenas um membro pode ser acessado por vez
- permite uma maneira eficiente de usar o mesmo espaço de memória 
para diferentes propósitos, dependendo da necessidade do momento

ENUM
- conjunto de constantes inteiras nomeadas
- usado para atribuir nomes a constantes inteiras, 
tornando o código mais legível e fácil de manter. 
- Diferente de union, um enum não é sobre armazenamento ou 
representação de diferentes tipos de dados, 
mas sobre a criação de um tipo de dado com 
valores específicos predefinidos.*/

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  union timer_status_field_val conf;

  switch (field) {
    case tsf_all: //configuration/status
      conf.byte = st; //status
      break;
    case tsf_initial: //timer initialization mode
      // Bits 4 e 5 (LSB e MSB), indicam como o valor inicial do timer é carregado
      conf.in_mode = (st & (TIMER_LSB | TIMER_MSB)) >> 4;
      /* >> 4 desloca os Bits 4 e 5 para as posições 0 e 1,
      deixando o valor facilmente comparado ou atribuído.*/
      break;

    case tsf_mode: //timer counting mode
      // Bits 1-3
      conf.count_mode = (st & TIMER_SQR_WAVE) >> 1; //counting mode 0..5
      // os modos "6" e "7" tem a mesma representação que 2 e 3
      // com & 0x3, os modos 6 e 7 são convertidos para 2 e 3
      if (conf.count_mode > 5) conf.count_mode &= 0x3; 

      break;
    case tsf_base: // timer counting base
      conf.bcd = st & TIMER_BCD; //counting base, true if BCD
      break;

    default:
      return 1;
  }

  if (timer_print_config(timer, field, conf) != 0) return 1;
  return 0;
}

