#include <lcom/lcf.h>

#include <stdint.h>

#ifdef LAB3
uint32_t sys_counter = 0;
#endif

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) return 1;  // Verificar se o ponteiro não é NULL
  *lsb = (uint8_t)(val & 0xFF);  // seleciona o byte menos significativo
  
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) return 1;  // Verificar se o ponteiro não é NULL
  *msb = (uint8_t)(val >> 8);  // seleciona o byte mais significativo

  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL) return 1;  // Verificar se o ponteiro não é NULL
  
  uint32_t temp_value;
  if (sys_inb(port, &temp_value) != OK) return 1;  // Chamada de sistema sys_inb
  sys_counter++;
  util_get_LSB(temp_value, value); // Chama a função para obter o LSB de val

  return 0;
}
