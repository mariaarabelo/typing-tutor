#ifndef _I8042_H_
#define _I8042_H_

#include <lcom/lcf.h>

/** @defgroup I8042 I8042
 * @{
 *
 * Constants for programming the I8042 KBC. Needs to be completed.
 */

#define KEYBOARD_IRQ 1 /**< @brief Keyboard 1 IRQ line */

#define KEYBOARD_BIT BIT(1)
#define DELAY_US 20000

#define BREAK BIT(7)

//KEYS
#define KEY_ESC 0x01
#define KEY_A 0x1E
#define KEY_B 0x30
#define KEY_C 0x2E
#define KEY_D 0x20
#define KEY_E 0x12
#define KEY_F 0x21
#define KEY_G 0x22
#define KEY_H 0x23
#define KEY_I 0x17
#define KEY_J 0x24
#define KEY_K 0x25
#define KEY_L 0x26
#define KEY_M 0x32
#define KEY_N 0x31
#define KEY_O 0x18
#define KEY_P 0x19
#define KEY_Q 0x10
#define KEY_R 0x13
#define KEY_S 0x1F
#define KEY_T 0x14
#define KEY_U 0x16
#define KEY_V 0x2F
#define KEY_W 0x11
#define KEY_X 0x2D
#define KEY_Y 0x15
#define KEY_Z 0x2C

#define KEY_ENTER 0x1c
#define KEY_SPACE 0x39
#define KEY_DELETE 0x0e

#define KEY_COMMA 0x33
#define KEY_PERIOD 0x34

/**@}*/

#endif /* _LCOM_I8042_H */
