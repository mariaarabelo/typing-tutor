#include "sprite.h"

Sprite *create_sprite_xpm(xpm_map_t sprite, uint16_t x, uint16_t y){

  Sprite *sp = (Sprite *) malloc (sizeof(Sprite));
  if( sp == NULL ) return NULL;

  xpm_image_t img;
  sp->colors = (uint32_t *) xpm_load(sprite, XPM_8_8_8_8, &img);
  sp->height = img.height;
  sp->width = img.width;
  sp->x = x;
  sp->y = y;

  if( sp->colors == NULL ) {
    free(sp);
    return NULL;
  }
  return sp;
}

void destroy_sprite(Sprite *sprite) {
    if (sprite == NULL) return;
    if (sprite->colors) free(sprite->colors);
    free(sprite);
    sprite = NULL;
}
