#ifndef _MODEL_H_
#define _MODEL_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>
#include "../drivers/graphics/video.h"
#include "../drivers/keyboard/keyboard.h"
#include "../drivers/mouse/mouse.h"
#include "../drivers/rtc/rtc.h"
#include "model/sprite.h"
#include "../view/xpm/letters.xpm"
#include "../view/xpm/cursor.xpm"
#include "../view/xpm/star.xpm"

#include "../view/xpm/play.xpm"
#include "../view/xpm/instructions.xpm"
#include "../view/xpm/highscore.xpm"

#include "../view/xpm/timer15.xpm"
#include "../view/xpm/timer30.xpm"
#include "../view/xpm/timer60.xpm"

#include "../view/xpm/punctuation/comma.xpm"
#include "../view/xpm/punctuation/period.xpm"
#include "../view/xpm/punctuation/colon.xpm"
#include "../view/xpm/punctuation/exclamation.xpm"
#include "../view/xpm/punctuation/right_parenthesis.xpm"

#include "../view/xpm/numbers.xpm"

#include "../view/xpm/panda.xpm"
#include "../view/xpm/bambu_right.xpm"
#include "../view/xpm/bambu_left.xpm"

#include "../view/xpm/back_to_menu.xpm"
#include "../view/xpm/play_again.xpm"

#include "../view/xpm/animation/panda0.xpm"
#include "../view/xpm/animation/mad1.xpm"
#include "../view/xpm/animation/mad2.xpm"
#include "../view/xpm/animation/mad3.xpm"
#include "../view/xpm/animation/mad4.xpm"
#include "../view/xpm/animation/happy1.xpm"
#include "../view/xpm/animation/happy2.xpm"
#include "../view/xpm/animation/happy3.xpm"
#include "../view/xpm/animation/happy4.xpm"

#define MAX_WORDS 40
#define MAX_WORD_LENGTH 20

#define KEY_SPRITE_MAP_SIZE 43

#define MAX_HIGH_SCORES 10

#define MAX_HIGH_SCORES 10

/**
 * @defgroup model
 * @{
 * Functions related to the game model and its respective logic
*/


/**
 * @brief Enumerates the Keyboard keys
*/
typedef enum{
    NONE_KEY,
    ENTER,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    COMMA,
    PERIOD,
    COLON,
    EXCLAMATION,
    RIGHT_PARENTHESIS,
    ZERO,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    BACK,
    ESC
} Key;

/**
 * @brief Enumerates the possible states of game states
*/
typedef enum{
    MENU,
    INSTRUCTIONS,
    STATISTICS,
    GAME,
    TIMERS,
    HIGHSCORES
} GameState;


/**
 * @brief Structure that represents the caret (Line under the current letter)
 * @param x the x position of the caret
 * @param y the y position of the caret
 * @param width the width of the caret
 * @param height the height of the caret
*/
typedef struct{
    int x;
    int y;
    int width;
    int height;
} Caret;


/**
 * @brief Structure that represents a letter
 * @param character the character itself
 * @param status the status of the letter: 0 for not typed, 1 for typed, -1 for incorrect
*/
typedef struct {
    char character;  // The character itself
    int status;      // Status of the letter: 0 for not typed, 1 for typed, -1 for incorrect
} Letter;



/**
 * @brief Structure that represents a word
 * @param letters the array of letters for the current word
 * @param length the length of the word
 * @param status the status of the word: 0 for not typed, 1 for correct, -1 for incorrect
 * @param x the x position of the word
 * @param y the y position of the word
 * @param line the line of the word
*/
typedef struct {
    Letter letters[MAX_WORD_LENGTH];
    int length;
    int status;
    int x;
    int y;
    int line;
} Word;



/**
 * @brief Has the structures to save the statistics of the game
 * @param correctWords the number of correct words typed
 * @param incorrectWords the number of incorrect words typed
 * @param correctLetters the number of correct letters typed
 * @param incorrectLetters the number of incorrect letters typed
 * @param typedLetters the number of letters typed
 * @param typedWords the number of words typed
 * @param time the time taken to type the words
*/
typedef struct {
    int correctWords;
    int incorrectWords;
    int correctLetters;
    int incorrectLetters;
    int typedLetters;
    int typedWords;
    int time;
} Statistics;


/**
 * @brief Has the structures to save the words and user input
 * @param words the words to be typed
 * @param wordCount the number of words
 * @param currentWordIndex the index of the current word
 * @param currentInputIndex the index of the current input
 * @param currentInput the current input
 * @param number_of_lines the number of lines in the test
*/
typedef struct {
    Word *words;
    int wordCount;
    int currentWordIndex;
    int currentInputIndex;
    char currentInput[MAX_WORD_LENGTH];
    int number_of_lines;
} TypingTest;


/**
    * @brief Enumerates the possible states of the swipe-right gesture
*/
typedef enum {
    GESTURE_ZERO,
    GESTURE_LB
} GestureState;

Sprite *CURSOR_SPRITE;

Sprite *STAR_SPRITE;

Sprite *PLAY_SPRITE;
Sprite *INSTRUCTIONS_SPRITE;
Sprite *HIGHSCORES_SPRITE;

Sprite *TIMER15_SPRITE;
Sprite *TIMER30_SPRITE;
Sprite *TIMER60_SPRITE;

Sprite *BACK_TO_MENU_SPRITE;
Sprite *PLAY_AGAIN_SPRITE;
Sprite *BACK_TO_MENU_HIGHSCORE_SPRITE;

Sprite *A_SPRITE;
Sprite *B_SPRITE;
Sprite *C_SPRITE;
Sprite *D_SPRITE;
Sprite *E_SPRITE;
Sprite *F_SPRITE;
Sprite *G_SPRITE;
Sprite *H_SPRITE;
Sprite *I_SPRITE;
Sprite *J_SPRITE;
Sprite *K_SPRITE;
Sprite *L_SPRITE;
Sprite *M_SPRITE;
Sprite *N_SPRITE;
Sprite *O_SPRITE;
Sprite *P_SPRITE;
Sprite *Q_SPRITE;
Sprite *R_SPRITE;
Sprite *S_SPRITE;
Sprite *T_SPRITE;
Sprite *U_SPRITE;
Sprite *V_SPRITE;
Sprite *W_SPRITE;
Sprite *X_SPRITE;
Sprite *Y_SPRITE;
Sprite *Z_SPRITE;
Sprite *COMMA_SPRITE;
Sprite *PERIOD_SPRITE;

Sprite *EXCLAMATION_SPRITE;
Sprite *COLON_SPRITE;
Sprite *RIGHT_PARENTHESIS_SPRITE;

Sprite *ZERO_SPRITE;
Sprite *ONE_SPRITE;
Sprite *TWO_SPRITE;
Sprite *THREE_SPRITE;
Sprite *FOUR_SPRITE;
Sprite *FIVE_SPRITE;
Sprite *SIX_SPRITE;
Sprite *SEVEN_SPRITE;
Sprite *EIGHT_SPRITE;
Sprite *NINE_SPRITE;

Sprite *PANDA_SPRITE;
Sprite *BAMBU_RIGHT_SPRITE;
Sprite *BAMBU_LEFT_SPRITE;

Sprite* PANDA_0_SPRITE;
Sprite* MAD_1_SPRITE;
Sprite* MAD_2_SPRITE;
Sprite* MAD_3_SPRITE;
Sprite* MAD_4_SPRITE;
Sprite* HAPPY_1_SPRITE;
Sprite* HAPPY_2_SPRITE;
Sprite* HAPPY_3_SPRITE;
Sprite* HAPPY_4_SPRITE;

Animation happyAnimation;
Animation madAnimation;

Sprite **happyFrames;
Sprite **madFrames;

typedef struct {
    int wpm;
    real_time_info achieved_time; // RTC time info when the score was achieved
    uint8_t day;    // Add day field
    uint8_t month;
} HighScore;

extern HighScore highScores[MAX_HIGH_SCORES];

/**
 * @brief Initializes the key maps
*/
void initialize_key_maps();

/**
 * @brief Initializes the sprites
*/
void initialize_sprites();

/**
 * @brief Initializes the key sprite map
*/
void initialize_key_sprite_map();

/**
 * @brief Free the memory allocated for the sprites
*/
void destroy_sprites();

/**
 * @brief Assigns tasks to the respective keys
*/
void update_keyboard();

/**
 * @brief handles the timer interrupts
*/
void update_timer();

/**
 * @brief Handles the keyboard interrupts
*/
void key_handler();

/**
 * @brief Frees the memory allocated for the typing test
*/
void destroy_test();

/**
 * @brief Frees the memory allocated for the statistics
*/
void destroy_stats();

/**
* @brief Updates currentState and gameStateChange
*/
void setGameState(GameState state);

/**
 * @brief Resets offset to starting of box
*/
void reset_offset();

/**
 * @brief Handles the offset for drawing
 * @param x the offset to be handled
 * @return the new offset
*/
int offset_handler(int x, int end_x);

/**
 * @brief Sync mouse bytes and calls update_mouse_date
*/
void update_mouse();


/**
 * @brief Checks if the mouse sprite is over a button and if left button is pressed
 * @param button_sprite the sprite of the button
 * @return 1 if the button was pressed, 0 otherwise
*/
int pressed_button(Sprite *button_sprite);

/**
 * @brief Checks the actions to be taken based on the mouse data
*/
void checkActions();

/**
 * @brief Gets a key based on a char
*/
Key char_to_key(char c);

/**
 * @brief Updates the swipe-right gesture status
*/
void checkGesture();

/**
 * @brief Fills correctly the remaining letters of the current word
*/
void fill_current_word();


/**
 * @brief Processes the key pressed, updating the letter status and the current input, and also the statistics
 * @param c the character pressed
 * @param key the key pressed
 * @param test the typing test
*/
void process_key(char c, Key key, TypingTest *test, GameState state);

/**
 * @brief Checks if the current word is correct and advances to the next word, also updates the statistics
 * @param test the typing test
*/
void handle_space_key(TypingTest *test);

void initialize_high_scores();

void update_high_scores(int wpm, real_time_info achieved_time);

void save_high_scores();

void load_high_scores();

/**
 * @brief Goes back to the previous letter and resets the current letter status
 * @param test the typing test
 */
void handle_delete_key(TypingTest *test);


/**
 * @brief Updates the frame of the animation and respective active status
 * @param animation the animation to be updated
 */
void updateAnimation(Animation *animation);



/**
 * @brief Checks if the gesture to fill the current word is correct
*/
void checkGesture();


/**
 * @brief Moves the penultimate line to first line, last line to second and loads new words
 * @param test the typing test
*/
void shift_words_up(TypingTest *test);


/**
 * @brief Loads new words to the test
 * @param test the typing test
 * @param index the index where to load the word
*/
void load_new_word(TypingTest *test, int index);


/**
 * @brief Starts the animation
 * @param animation the animation to be started
*/
void startAnimation(Animation *animation);


/**
 * @brief Initializes the test
 * @param testPtr the pointer to the test
 * @param wordPool the pool of words
 * @param poolSize the size of the pool
 * @param wordCount the number of words
*/
void initializeTest(TypingTest **testPtr, char *wordPool[], int poolSize, int wordCount);

#endif

