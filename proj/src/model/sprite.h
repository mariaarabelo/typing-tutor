#ifndef _SPRITE_H_
#define _SPRITE_H_

#
#include "../drivers/graphics/video.h"


/**
 * @defgroup sprite
 * @{
 * Functions related to sprites and animations, visual representation of the items in the game
*/

/**
 * @brief Struct that represents a sprite
 * @param height the height of the sprite
 * @param width the width of the sprite
 * @param x the x position of the sprite (Only used for buttons)
 * @param y the y position of the sprite (Only used for buttons)
 * @param colors the colors of the sprite
 * @param color the color of the sprite
*/
typedef struct {
    uint16_t height;
    uint16_t width;
    int x;
    int y;
    uint32_t *colors;
    uint32_t color;
} Sprite; 



/**
 * @brief Struct that represents an animation
 * @param frames the frames of the animation
 * @param frameCount the number of frames
 * @param currentFrame the current frame
 * @param isActive if the animation is active
 * @param frameDuration the duration of each frame
 * @param frameCounter the counter of the frames
*/
typedef struct {
    Sprite **frames;
    int frameCount;
    int currentFrame; 
    bool isActive; 
    int frameDuration;
    int frameCounter;
} Animation;


/**
 * @brief Creates a sprite from an xpm
 * @param sprite the xpm of the sprite
 * @param x the x position of the sprite
 * @param y the y position of the sprite
*/
Sprite *create_sprite_xpm(xpm_map_t sprite, uint16_t x, uint16_t y);


/**
 * @brief Destroys a sprite
 * @param sprite the sprite to be destroyed
*/
void destroy_sprite(Sprite *sprite);

#endif

