#ifndef __MOUSE_H
#define __MOUSE_H

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"

/** @defgroup mouse
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
int(mouse_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(mouse_unsubscribe_int)();

/**
 * @brief Enables stream mode mouse data reporting
 *
 * sends the respective command to the mouse
 * 
 * @return Return 0 upon success and non-zero otherwise
 */
int (mouse_enable_data_reporting)(void);
 
 
/**
 * @brief Handles mouse interrupts
 *
 * Reads the st reg and the output buffer
 * if there was some error, the byte read from the OB is discarded
 * 
 */
void (mouse_ih)(void);

/**
 * @brief Parse packet
 * 
*/
void (parse)(void);

/**
 * @brief sync packet
 * 
*/
void (sync)(void);

/**
 * @brief Sends a command to the mouse
 * @param cmd command to be sent
 * @return Return 0 upon success and non-zero otherwise
 */ 
int (mouse_command)(uint32_t cmd);

/**
 * @brief Checks if the output buffer is full
 * 
 * so we know if we can read from it
 * 
 * @return true if the output buffer is full, false otherwise
*/
bool (obf_full)();

/**
 * @brief Checks if the input buffer is empty
 * 
 * so we know if we can write to it
 * 
 * @return true if the input buffer is empty, false otherwise
*/
bool (ibf_empty)();

/**
 * @brief Process state based on the packet
 * 
 * @param pp packet to be processed
 * @param x_len length of the x movement
 * @param tolerance tolerance of the movement
 * 
*/
void (process_packet)(struct packet *pp, uint8_t x_len, uint8_t tolerance);

enum state {
    STATE_ZERO, // 0 
    STATE_INIT, // 1
    STATE_FIRST_LINE, // 2
    STATE_END_FIRST_LINE, // 3
    STATE_VERTEX, // 4
    STATE_SECOND_LINE, // 5
    STATE_END_SECOND_LINE, // 6
    STATE_END // 7
};

#endif /* __MOUSE_H */
