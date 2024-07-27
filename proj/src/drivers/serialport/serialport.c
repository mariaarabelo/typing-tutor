#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

#include <lcom/utils.h>
#include "serialport.h"

int sp_hook_id = 1;
uint8_t data = 0x00;
bool ready = false;


int (sp_init)(){
    if(sys_outb(SP_ADR + SP_OFFSET_INT, SP_RECEIVE)){
        printf("Error writing from Serial Port to Interrupt Register");
        return 1;
    }
    while(!sp_get_data()){
        printf("No useful data");
    }
    sp_send_data(SP_CONNECT);
    return 0;
}

int (sp_subscribe_int) (uint8_t *bit_no){
    sp_hook_id = *bit_no;
    return sys_irqsetpolicy(SP_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &sp_hook_id);
}

int (sp_unsubscribe_int)(){
    return sys_irqrmpolicy(&sp_hook_id);
}

int (sp_get_status)(uint8_t* status){
    if(util_sys_inb(SP_ADR + SP_OFFSET_LINE,status) != 0){
        printf("Error writing from Serial Port to Line Register");
        return 1;
    }
    if(*status & SP_OVERRUN){
        printf("Overrun error");
        return 1;
    }
    if(*status & SP_PARITY){
        printf("Parity error");
        return 1;
    }
    if(*status & SP_FRAME){
        printf("Frame error");
        return 1;
    }
    return 0;
}

int (sp_get_data)(){
    uint8_t status;
    sp_get_status(&status);

    if(!(status & SP_DATAR)){
        data = 0;
        printf("Data unready");
        return 1;
    }
    while(status & SP_DATAR){
        if(util_sys_inb(SP_ADR + SP_RR, &data)){
            printf("Error receiving data");
            return 1;
        }
        printf("Data: %d\n", data);
        sp_get_status(&status);
    }
    return 0;
}

int (sp_send_data)(uint8_t data){
    uint8_t status;
    while(true){
        sp_get_status(&status);
        if( status & (SP_REGREADY | SP_REGEMPTY)){
            if(sys_outb(SP_ADR + SP_TR, data )){
                printf("Error reading data");
                return 1;
            }
            printf("Data: %d\n", data);
            return 0;
        }
        tickdelay(micros_to_ticks(5));
    }
}

int (sp_connection)(){
    if(data == SP_CONNECT){
        ready = true;
        sp_send_data(SP_START);
        return SP_CONNECT;
    }
    if(data == SP_START){
        return SP_START;
    }
    return 0;
}
