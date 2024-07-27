#ifndef __SERIALPORT_H_
#define __SERIALPORT_H_

#include <lcom/lcf.h>
#include <stdbool.h>
#include <stdint.h>

#define SP_ADR 0x3F8 /* address */
#define SP_IRQ 4 /* IRQ line */

#define SP_CONNECT 2 /* message on connect */
#define SP_START 3 /* message on start */

#define SP_OFFSET_INT 1 /* interrupt enable register offset for the address */
#define SP_RECEIVE BIT(0) /* enable data interrupts */

#define SP_OFFSET_LINE 5 /* status register offset for the address */
#define SP_DATAR BIT(0) /* data available */
#define SP_OVERRUN BIT(1) /* overrun error */
#define SP_PARITY BIT(2) /* parity error*/ 
#define SP_FRAME BIT(3) /* framing error */
#define SP_REGREADY BIT(5) /* register ready for data */
#define SP_REGEMPTY BIT(6) /* register empty */

#define SP_RR 0 /* receive register offset */
#define SP_TR 0 /* transmitter regiter offset */

int (sp_init)();

int (sp_subscribe_int)(uint8_t *bit_no);

int (sp_unsubscribe_int)();

int (sp_connection)();

int (sp_get_data)();

int (sp_get_status)(uint8_t* status);

int (sp_send_data)(uint8_t data);

#endif 
