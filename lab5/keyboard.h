#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>

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
int(keyboard_subscribe_int)(uint8_t *bit_no);

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
 * @brief Waits for the ESC key to be released
 * @return Return 0 upon success and non-zero otherwise
*/
int (wait_ESC)();

#endif /* __KEYBOARD_H */
