#include <lcom/lcf.h>

#include <stdint.h>
#include "mouse.h"

static int mouse_hook_id = 3; 
uint8_t byte_received;
struct packet pp;
int bytes_read = 0;
uint8_t pp_bytes[3];
enum state state = STATE_ZERO;
uint8_t abs_x_len = 0;
uint8_t abs_y_len = 0;
    
    
int (mouse_subscribe_int)(uint8_t *bit_no) {
 
  if (bit_no==NULL) return 1;
  *bit_no = BIT(mouse_hook_id); 

  if (sys_irqsetpolicy(12, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != OK)
    return 1;
  // IRQ_REENABLE (int, the policy) so that the generic
  // interrupt handler will acknowledge the interrupt,
  // output the EOI command to the PIC
  // enabling further interrupts on the corresponding IRQ line
  return 0;
}

int (mouse_unsubscribe_int)() {
  if (sys_irqrmpolicy(&mouse_hook_id)!=OK)
    return 1; 
  return 0;
}

// 0xF4, ativa o data report;
// 0xF5, desativa o data report;
// 0xEA, ativa o stream mode;
// 0xF0, ativa o remote mode; polling 
// 0xEB, manda um request de novos dados;

void (mouse_ih)(){    
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

    //if bit(0)==1, OBF is full
    if ((st & BIT(0)) && (st & BIT(5))){ // if the output buffer is full, we can read it
      if (util_sys_inb(0x60 , &byte_received) != OK){
        printf("this byte should be discarded \n");  
        return;
      }
      // otherwise, we successfully read the byte
      return;
    }
}

void (parse)(){
  pp.bytes[0] = pp_bytes[0];
  pp.bytes[1] = pp_bytes[1];
  pp.bytes[2] = pp_bytes[2];

  pp.rb = pp_bytes[0] & BIT(1); 
  pp.mb = pp_bytes[0] & BIT(2);
  pp.lb = pp_bytes[0] & BIT(0);
  
  if (pp_bytes[0] & BIT(4)){
      pp.delta_x = pp_bytes[1] | 0xFF00; 
  } else {
      pp.delta_x =pp_bytes[1];
  }

  if (pp_bytes[0] & BIT(5)){
      pp.delta_y = pp_bytes[2] | 0xFF00; 
  } else {
      pp.delta_y = pp_bytes[2];
  }
  
  pp.x_ov = pp_bytes[0] & BIT(6);
  pp.y_ov = pp_bytes[0] & BIT(7);
}

void (sync)(){  
  if (bytes_read == 0){
      if (byte_received & BIT(3)){
          bytes_read++;
          pp_bytes[0] = byte_received;
      };
  } else if (bytes_read == 1){
      pp_bytes[1] = byte_received;
      bytes_read++;
  } else if (bytes_read == 2){
      pp_bytes[2] = byte_received;
      bytes_read++;
  }
}

bool (ibf_empty)(){
  uint8_t st;
  int tries = 10;
  while (tries > 0){
    tries--;
    // tickdelay(micros_to_ticks(DELAY_US));
    if (util_sys_inb(0x64 , &st) != OK) // lê o status register
    {
      printf("reading status register 0x64 was not ok\n");  
      continue;
    }

    if (st & (BIT(7))) {
      printf("Parity Error.\n");
      continue;
    }
    
    if (st & (BIT(6))) {
      printf("Timeout Error.\n");
      continue;
    }

    if (!(st & BIT(1))){ // bit 1 não está setado, IBF está vazio
      // printf("checking ibf %d\n", tries);
      return true;
    } // if bit(1), IBF is not empty
  }
  printf("IBF is not empty. \n");
  return false;
}
  
bool (obf_full)() {
  uint8_t st;
  int tries =3;
  while (tries>0){
    tries--;
    // tickdelay(micros_to_ticks(DELAY_US));
    if (util_sys_inb(0x64 , &st)){ // lê o status register
      continue;
    } 

    if (st & (BIT(7))) {
      printf("Parity Error.\n");
      continue;
    }
    
    if (st & (BIT(6))) {
      printf("Timeout Error.\n");
      continue;
    }    

    if (st & BIT(0)) {
      //if (st & BIT(5)){
        printf("OBF is full and AUX is set\n");
        return true;
      //}
    }
  }
  printf("OBF is not full. \n");
  return false;  
}

int (mouse_command)(uint32_t cmd) {
    int tries=10;
    uint8_t response;
    while (tries>0){
      tries--;
      
      if (!ibf_empty()) {
        continue;
      }
      if (sys_outb(0x64, 0xD4)){ // write byte mouse command
          printf("writing 0xD4 to 0x64 was not ok. \n");  
          continue;
      }
      if (!ibf_empty()) {
        continue;
      }
      printf("writing cmd %X to 0x60\n", cmd);
      if (sys_outb(0x60, cmd)){ // enable/disable data reporting command
          printf("writing cmd %x to 0x60 was not ok. \n", cmd);  
          continue;
      }
      
      tickdelay(micros_to_ticks(DELAY_US));
      // if (!obf_full()) {
      //   continue;
      // }
      // printf("obf full\n");

      if (util_sys_inb(0x60, &response)){
          printf("reading response from 0x60 was not ok. \n"); 
          continue; 
      }
      if (response == 0xFA){ 
        printf("Mouse acknowledged command.\n");
        return 0; 
      } else {
          printf("response was not a success case. \n");  
          printf("response was %d\n", response);
          if (response == 0xFE){ // NACK
              printf("response was 0xFE, send the entire command again! \n");  
          }
      }
    }
    printf("Mouse did not acknowledge command.\n");


    return 1;
}

void (process_packet)(struct packet *pp, uint8_t x_len, uint8_t tolerance){
  switch (state){
    case STATE_ZERO:
        if (!pp->lb && !pp->rb && !pp->mb){
            printf("no buttons pressed\n");
            state = STATE_INIT;
        }
        break;
    case STATE_INIT:
        abs_x_len = 0;
        abs_y_len = 0;    
        if (pp->lb && !pp->rb && !pp->mb){
            printf("left button was pressed\n");
            state = STATE_FIRST_LINE;
        }
        break;
    case STATE_FIRST_LINE:
        if ((!(pp->delta_x & BIT(15)) || (pp->delta_x & BIT(15) && abs(pp->delta_x) <= tolerance)) && pp->lb && !pp->rb && !pp->mb){ 
            if (!(pp->delta_y & BIT(15)) || (pp->delta_y & BIT(15) && abs(pp->delta_y) <= tolerance)){
                abs_x_len += pp->delta_x; 
                abs_y_len += pp->delta_y;
                printf("abs_x_len %d, abs_y_len %d\n", abs_x_len, abs_y_len);
                if (abs_x_len >= x_len && abs_y_len > abs_x_len + 1){
                    printf("first line was drawn\n");
                    state = STATE_END_FIRST_LINE;
                }
                break;
            } 
        } 
        if (!pp->lb && !pp->rb && !pp->mb){
            printf("right button was released, going to init state\n");
            state = STATE_INIT;
            break;
        }
        printf("problems in drawing first line, going back to state_zero\n");
        state = STATE_ZERO;
        break;
    case STATE_END_FIRST_LINE:
        if (!pp->lb && !pp->rb && !pp->mb){
            printf("left button was released\n");
            state = STATE_VERTEX;
            break;
        }
        if (pp->rb || pp->mb){
            printf("right or middle button was pressed :(\n");
            state = STATE_ZERO;
            break;
        }
        break;
    case STATE_VERTEX: // 4
        abs_x_len = 0;
        abs_y_len = 0;            
        if (pp->rb && !pp->lb && !pp->mb){
            printf("right button was pressed, starting 2nd line\n");
            state = STATE_SECOND_LINE;
            break;
        }
        if (pp->lb && !pp->rb && !pp->mb){
            printf("left button was pressed, starting the 1st line again\n");
            state = STATE_FIRST_LINE;
            break;
        }
        if (abs(pp->delta_x) <= tolerance && abs(pp->delta_y) <= tolerance &&  !pp->lb && !pp->rb && !pp->mb){
            break; // stay in vertex state
        }

        printf("problems in vertex, going back to state_zero\n");
        state = STATE_ZERO;
        break;
    case STATE_SECOND_LINE: // 5
        if ((!(pp->delta_x & BIT(15)) || (pp->delta_x & BIT(15) && abs(pp->delta_x) <= tolerance)) && pp->rb && !pp->lb && !pp->mb){ 
            if ((pp->delta_y & BIT(15)) || (!pp->delta_y & BIT(15) && pp->delta_y <= tolerance)){
                abs_x_len += pp->delta_x; 
                abs_y_len += pp->delta_y;
                if (abs_x_len >= x_len && abs_y_len > abs_x_len + 1){
                    printf("second line was drawn\n");
                    state = STATE_END_SECOND_LINE;
                }
                break;
            } 
        } 
        if (!pp->lb && !pp->rb && !pp->mb){
            printf("left button was released, going to init state\n");
            state = STATE_INIT;
            break;
        }
        printf("problems in drawing second line, going back to state_zero\n");
        state = STATE_ZERO;
        break;
    case STATE_END_SECOND_LINE: //6 
        if (!pp->lb && !pp->rb && !pp->mb){
            printf("right button was released\n");
            state = STATE_END;
            break;
        }
        if (pp->lb || pp->mb){
            printf("left or middle button was pressed :(\n");
            state = STATE_ZERO;
            break;
        }
        break;
    case STATE_END:
        printf("end of gesture\n");
        break;
  }
}
