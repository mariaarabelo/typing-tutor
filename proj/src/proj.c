#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "drivers/keyboard/keyboard.h"
#include "drivers/timer/timer.h"
#include "drivers/mouse/mouse.h"
#include "drivers/rtc/rtc.h"
#include "model/model.h"
#include "model/sprite.h"
#include "view/view.h"

extern Key currentKey;
extern uint8_t *main_frame_buffer;
extern uint8_t *secondary_frame_buffer;
extern uint32_t frame_size;
extern real_time_info time_info;
extern int counter;

extern TypingTest *test;
extern GameState currentState;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  //lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/src/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int setup(){
  // mouse stuff
  if (mouse_command(0xF4)) return 1; // enable mouse data reporting
  if (mouse_subscribe_int()) return 1;

  if(timer_set_frequency(0,60)!=0) return 1;
  if(setUpFrameBuffer()!=0) return 1;
  if (set_graphic_mode(0x14C) != 0) return 1;

  if(keyboard_subscribe_int()!=0) return 1;
  if(timer_subscribe()!=0) return 1;

  if(rtc_subscribe_interrupts()!=0) return 1;
  
  initialize_sprites();
  initialize_key_maps();
  initialize_key_sprite_map();

  setGameState(MENU);

  return 0;
}



int end(){
  /* DEBUGGING */


  //destroy_test();   // SHOULD BE CALLED WHEN LEAVING THE GAME MODE, HERE TO AVOID MEMORY LEAKS DURING DEVELOPMENT
                    // LEAVING THE APPLICATION WITHOUT STARTING A GAME WILL CAUSE A CRASH BECAUSE OF THIS
  destroy_sprites();

  if(keyboard_unsubscribe_int()!=0) return 1;
  if(timer_unsubscribe_int()!=0) return 1;
  if(vg_exit()!=0) return 1;
  if(rtc_unsubscribe_interrupts()!=0) return 1;
  

  // mouse stuff 
  if (mouse_unsubscribe_int()) return 1;
  if (mouse_command(0xF5)) return 1; // disable data reporting

  return 0;
}

int (proj_main_loop)(int argc, char **argv) {

  setup();

  int ipc_status;
  message msg;
  while (!(currentState==MENU && currentKey == ESC)) {
    
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("Error");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: 
          if (msg.m_notify.interrupts & KEYBOARD_BIT){
            key_handler();
            
          }
          if (msg.m_notify.interrupts & TIMER_BIT) {
            update_timer();
            GameDrawer();
          }

          if (msg.m_notify.interrupts & MOUSE_BIT) {
            update_mouse();
          }
        }
    }
  }

  if (end()) return 1;

  return 0;
}

