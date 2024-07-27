#include <lcom/lcf.h>
#include "video.h"
#include <math.h>

int (set_graphic_mode)(uint16_t mode) {
    reg86_t reg86;
    memset(&reg86, 0, sizeof(reg86));
    reg86.intno = 0x10;
    reg86.ah = 0x4F;  
    reg86.al = 0x02;                
    reg86.bx = mode | BIT(14);
    if (sys_int86(&reg86) != 0) {   
        printf("Error setting graphic mode\n");
        return 1;
    }
    return 0;
}

int (set_frame_buffer)(uint16_t mode) {

  memset(&mode_info, 0, sizeof(mode_info));
  if(vbe_get_mode_info(mode, &mode_info)) return 1;

  unsigned int frame_size = (mode_info.XResolution * mode_info.YResolution * mode_info.BitsPerPixel) / 8;
  
  struct minix_mem_range physic_addresses;
  physic_addresses.mr_base = mode_info.PhysBasePtr;
  physic_addresses.mr_limit = physic_addresses.mr_base + frame_size;
  
  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &physic_addresses)) {
    printf("Physical memory allocation error\n");
    return 1;
  }

  main_frame_buffer = vm_map_phys(SELF, (void*) physic_addresses.mr_base, frame_size);
  if (main_frame_buffer == NULL) {
    printf("Virtual memory allocation error\n");
    return 1;
  }
  return 0;
}


int (draw_pixel)(uint16_t x, uint16_t y, uint32_t color, uint8_t* frame_buffer) {

  if(x >= mode_info.XResolution || y >= mode_info.YResolution) return 1;
  
  unsigned BytesPerPixel = (mode_info.BitsPerPixel + 7) / 8;
  unsigned int index = (mode_info.XResolution * y + x) * BytesPerPixel;

  memcpy(&frame_buffer[index], &color, BytesPerPixel);

  return 0;
}

int (draw_line)(uint16_t x, uint16_t y, uint16_t len, uint32_t color, uint8_t* frame_buffer) {
  for (unsigned i = 0 ; i < len ; i++)   
    if (draw_pixel(x+i, y, color, frame_buffer) != 0) return 1;
  return 0;
}

int (draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color, uint8_t* frame_buffer) {
  for(unsigned i = 0; i < height ; i++)
    if (draw_line(x, y+i, width, color, frame_buffer) != 0) {
      continue;
    }
  return 0;
}



