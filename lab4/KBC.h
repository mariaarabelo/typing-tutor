#include <stdint.h>
#include "i8042.h"

int READ_KBC_OUT(uint8_t port, uint8_t *output, uint8_t mouse);
int WRITE_KBC_CMD(uint8_t port, uint8_t command);
