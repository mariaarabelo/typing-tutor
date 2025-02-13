#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <lcom/lcf.h>
#include "VBE.h"

/** @defgroup video
 * @{
 * Functions for using the video card
 */ 


vbe_mode_info_t mode_info;

uint8_t *main_frame_buffer;
uint8_t *secondary_frame_buffer;
uint8_t *secondary_frame_buffer_no_mouse;

int (set_graphic_mode)(uint16_t submode);
int (set_frame_buffer)(uint16_t mode);
int (draw_pixel)(uint16_t x, uint16_t y, uint32_t color, uint8_t* frame_buffer);
int (draw_line)(uint16_t x, uint16_t y, uint16_t len, uint32_t color, uint8_t* frame_buffer);
int (draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color, uint8_t* frame_buffer);

#endif


