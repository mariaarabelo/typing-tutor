#ifndef __KEYBOARD_H_
#define __KEYBOARD_H_

#include <stdbool.h>
#include <stdint.h>
#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "i8042.h"

/** @defgroup keyboard
 * @{
 * Functions for using the i8042 KBC
 */ 


/**
 * @brief Subscribes and enables interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(keyboard_subscribe_int)();

/**
 * @brief Unsubscribes interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(keyboard_unsubscribe_int)();

/**
 * @brief Interrupt handler
*/
void (kbc_ih)();

/**
 * @brief reactivates the interrupts
*/
int (kbc_activate_interrupts)();

/**
 * @brief Polls the KBC
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int (kbc_polling)();

#endif /* __KEYBOARD_H */
