#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/** @defgroup I8042 I8042
 * @{
 *
 * Constants for programming the I8042 KBC. Needs to be completed.
 */

#define KEYBOARD_IRQ 1 /**< @brief Keyboard 1 IRQ line */

#define MOUSE_BIT BIT(3)

#define DELAY_US 20000

#define BREAK BIT(7)


/**@}*/



#endif /* _LCOM_I8042_H */
