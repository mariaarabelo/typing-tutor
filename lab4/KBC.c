#include "KBC.h"

int READ_KBC_OUT(uint8_t port, uint8_t *output, uint8_t mouse){

  uint8_t status;

  for (int i=0; i<10; i++){
    if(util_sys_inb(KBC_STATUS_REG,&status)!=0) return 1;

    if((status & KBC_OUT_BUF)!=0){ //OUTPUT FULL

      if(utils_sys_inb(port,output)!=0) return 1;

      if ((status&STATUS_PARITY_ERROR)!=0) return 1;

      if((status&STATUS_TIMEOUT_ERROR)!=0) return 1;

      if((!mouse) && ((status&STATUS_AUX)!=0)) return 1;

      if(mouse && ((status&STATUS_AUX)==0)) return 1;

      return 0;
    } 
    tick_delay(micros_to_ticks(20000));
  }

  return 1;
}


int WRITE_KBC_CMD(uint8_t port, uint8_t command){

  uint8_t status;

  for (int i=0;i<10; i++){

    if(utils_sys_inb(KBC_STATUS_REG,&status)!=0) return 1;

    if ((status&STATUS_INH)==0){ //INTPUT BUFFER EMPTY (READY TO READ)
      if (sys_outb(port,command)!=0) return 1;
      return 0;
    }

    tick_delay(micros_to_ticks(20000));
  }
  return 1;
}