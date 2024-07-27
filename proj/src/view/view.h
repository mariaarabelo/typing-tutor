#ifndef _VIEW_H_
#define _VIEW_H_


#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "../drivers/graphics/video.h"
#include "../model/sprite.h"
#include "../model/model.h"


/**
 * @defgroup view
 * @{
 * Functions related to the displaying of the game
*/


int setUpFrameBuffer();

/**
 * @brief Draws a sprite on the screen, secondary_frame_buffer_no_mouse
 * @param sprite the sprite to be drawn
 * @param x the x position of the sprite
 * @param y the y position of the sprite
 * @param single_color if the sprite is a single color
 * @param color the color of the sprite
 * @param moving if the sprite will be moving
 * @return 0 if successful, 1 otherwise
*/
int drawSpriteXPM(Sprite *sprite, int x, int y, bool single_color, uint32_t color, bool moving);

/**
 * @brief Draws a static sprite on the screen
 * @param sprite the sprite to be drawn
 * @return 0 if successful, 1 otherwise
*/
int drawStatic(Sprite *sprite);


/**
 * @brief Draws the background of the game
 * @param state the current state of the game
 * @return 0 if successful, 1 otherwise
*/
int drawBackground(GameState state);


/**
 * @brief Draws the record time on the screen
 * @return 0 if successful, 1 otherwise
*/
int drawRecordedTime();

/**
 * @brief Draws the remaining stars on the screen
 * @return 0 if successful, 1 otherwise
*/
int drawStars();

/**
 * @brief Displays the timers xpms on the screen
 * @return 0 if successful, 1 otherwise
*/
int drawTimers();

/**
 * @brief Draws the real-time clock on the screen.
 * @return 0 if successful, 1 otherwise
*/
int drawRealTime();


int drawStatistics();

/**
 * @brief Draws the cursor on the screen
 * @return 0 if successful, 1 otherwise
*/
int drawCursor();

/**
 * @brief Distributes the drawing of the game depending on the current state
 * @return 0 if successful, 1 otherwise
*/
int GameDrawer();

/**
 * @brief Draws a text on the screen
 * @param text the text to be drawn
 * @param color the color of the text
 * @return 0 if successful, 1 otherwise
*/
int drawText(const char* text, uint32_t color, int start_x, int end_x, int y);


/**
 * @brief Draws the game words on the screen
 * @param test the typing test
 * @return 0 if successful, 1 otherwise
*/
int drawWords(TypingTest *test);


/**
 * @brief Calculates the length of a word in pixels
 * @param word the word to be calculated
 * @return the length of the word in pixels
*/
int word_length_in_pixels(Word *word);


/**
 * @brief Deletes the caret from the screen
 * @return 0 if successful, 1 otherwise
*/
int deleteCaret();

/**
 * @brief Draws the caret on the screen
 * @param x the x position of the caret
 * @param y the y position of the caret
 * @return 0 if successful, 1 otherwise
*/
int drawCaret(int x,int y);


/**
 * Draws a integer on the screen
 * @param number the number to be drawn
 * @param x the x position of the number
 * @param y the y position of the number
 * @return 0 if successful, 1 otherwise
*/
int drawInt(int number, int x, int y);


/**
 * @brief Draws a animation depending on current frame etc...
 * @param animation the animation to be drawn
 * @param x the x position of the animation
 * @param y the y position of the animation
*/
void drawAnimation(Animation *animation, int x, int y);


/**
 * @brief Decides which animation to draw depending on their states
*/
void drawAnimationBoth();
int drawHighScores();

#endif
