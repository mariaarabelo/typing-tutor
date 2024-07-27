#include <lcom/lcf.h>

#include <stdint.h>
#include "video.h"

// MODE 
// 0x105 -> indexed
// 0x115 -> direct color

static char *video_mem;		/* Process (virtual) address to which VRAM is mapped 
                        frame-buffer address*/
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned bytes_per_pixel; /* Number of VRAM bytes per pixel */
static unsigned mem_model; /* Memory model used in the graphics mode */
static unsigned red_mask_size; /* Size of the red mask in bits */
static unsigned green_mask_size; /* Size of the green mask in bits */
static unsigned blue_mask_size; /* Size of the blue mask in bits */
static unsigned red_field_position; /* Bit position of the lsb of the red mask */
static unsigned green_field_position; /* Bit position of the lsb of the green mask */
static unsigned blue_field_position; /* Bit position of the lsb of the blue mask */

// static global -> not visible in other files
// akin static members in c++

int (map_vram)(uint16_t mode){
    vbe_mode_info_t vbe_mode_info;

    memset(&vbe_mode_info, 0, sizeof(vbe_mode_info)); 

    // 0x01 func -> return vbe mode info ; 
    //get vram physical addr from video controller
    // args: mode passed on CX, 
        // es:di real-mode (linear physical) addr of a 
        // buffer to be filled w params of specified mode
    if (vbe_get_mode_info(mode, &vbe_mode_info) !=0) 
        return 1; 
    
    phys_bytes vram_base_phys = vbe_mode_info.PhysBasePtr;

    h_res = vbe_mode_info.XResolution;
    v_res = vbe_mode_info.YResolution;
    
    printf("hres: %d\n", h_res);
    printf("vres: %d\n", v_res);

    bits_per_pixel = vbe_mode_info.BitsPerPixel;
    bytes_per_pixel = (bits_per_pixel+7)/8;

    unsigned int vram_size = h_res * v_res * bytes_per_pixel; // in bytes
    
    mem_model = vbe_mode_info.MemoryModel;

    red_mask_size = vbe_mode_info.RedMaskSize;
    green_mask_size = vbe_mode_info.GreenMaskSize;
    blue_mask_size = vbe_mode_info.BlueMaskSize;
    
    red_field_position = vbe_mode_info.RedFieldPosition;
    green_field_position = vbe_mode_info.GreenFieldPosition;
    blue_field_position = vbe_mode_info.BlueFieldPosition;

    struct minix_mem_range mr;
    mr.mr_base = vram_base_phys;
    mr.mr_limit = mr.mr_base + vram_size;

    if(sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)!=0)
        panic("sys_privctl (ADD_MEM) failed\n");
    // 1st arg: process that will have access to the memory

    // map graphics mode vram in the process address space
    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
    // endpoint_t proc_ep

    if (video_mem == MAP_FAILED) return 1;
    
    return 0;
}

int (vbe_set_mode)(uint16_t mode){
    //configure video card to graphics mode
    //use VESA VBE
    reg86_t reg86;
    memset(&reg86, 0, sizeof(reg86_t)); // clear the structure
    reg86.intno = 0x10; // bios video services INT number
    reg86.ax = 0x4F02; // AH = 4F - VBE call; 
                       // AL = 02 - func, set video mode
    reg86.bx = mode | BIT(14); 
    // bit 15 cleared - so display memory is cleared after 
        //switching to the desired graphics mode.
    // bit 14 -  linear frame buffer model -facilitates access to vram  
    // vram stores the pixels values
        
    // sys_int86 : temp switches (emulates) to 16-bit real mode to call bios services
    // executes the softw interrupt inst INT
    // processor jumps to the corresponding handler,
    // which must be configured at boot time
    // switch back to 32-bit protected mode
    if (sys_int86(&reg86)!=0) {
        printf("sys_int86 failed\n");
        return 1;
    }
    printf("Mode %x successfully set!\n", mode);
    return 0;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color){
    if (x >= h_res || y >= v_res) return 1;
    unsigned int pos = (y * h_res + x) * bytes_per_pixel; // in bytes
    for (unsigned int i = 0; i < bytes_per_pixel; i++){
        video_mem[pos+i] = (unsigned char)(color >> (i*8) & 0x0000FF); 
        // printf("color byte %02x\n", color >> (i*8) & 0xFF);
        // printf("video_mem byte %02x\n", (unsigned char) video_mem[pos+i]);
    }
    return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
    for (int j = x; j < x+len; j++){
        // printf("drawing pixel %d\n", j);
        if (vg_draw_pixel(j, y, color)!=0) return 1;
    }
    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, 
            uint16_t height, uint32_t color){
    
    // printf("color %x\n", color);
    if (bits_per_pixel != 32){
        color = color & ((1 << bits_per_pixel) - 1);
        // printf("bits per pixel %d\n", bits_per_pixel);
        // printf("color %x\n", color);
    }

    for (int i = y; i < y+height; i++){
        // printf("drawing line %d\n", i);
        if (vg_draw_hline(x, i, width, color)!=0) return 1;
    }
    return 0;
}

int (vg_draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step){
    
    uint32_t color = first;
    uint16_t width = h_res/no_rectangles;
    uint16_t height = v_res/no_rectangles;

    printf("width: %d\n", width);
    printf("height: %d\n", height);

    for (int i = 0; i < no_rectangles; i++){
        for (int j = 0; j < no_rectangles; j++){
            
            if (mem_model == 0x04){ // indexed 
                color = (first + (i * no_rectangles + j) * step) % (1 << bits_per_pixel);
            } else if (mem_model == 0x06){ // direct color

                // ((1 << red_mask_size) - 1) extrai os bits mais à direita
                uint32_t red_first = (first >> red_field_position) & ((1 << red_mask_size) - 1);
                uint32_t red = ((red_first >> red_field_position) + j * step) % (1 << red_mask_size);
                
                uint32_t green_first = (first >> green_field_position) & ((1 << green_mask_size) - 1);
                uint32_t green = ((green_first >> green_field_position) + i * step) % (1 << green_mask_size);
                
                uint32_t blue_first = (first >> blue_field_position) & ((1 << blue_mask_size) - 1);
                uint32_t blue = ((blue_first >> blue_field_position) + (i + j) * step) % (1 << blue_mask_size);

                color = (red << red_field_position) 
                        | (green << green_field_position) 
                        | (blue << blue_field_position);
                
            }
            
            vg_draw_rectangle(j*width, i*height, width, height, color);
        }
    }
    return 0;
}


int (vg_draw_img)(uint16_t x, uint16_t y, uint16_t width, 
            uint16_t height, uint8_t *sprite){

    // printf("img width: %d\n", width);
    // printf("img height: %d\n", height);

    for (int i = y; i<y+height; i++){
        // printf("NEW LINE\n");
        for (int j = x; j<x+width; j++){

            uint32_t color = *sprite
            ;
            // printf("color %x\n", color);
            if (bits_per_pixel != 32){
                color = color & ((1 << bits_per_pixel) - 1);
                // printf("bits per pixel %d\n", bits_per_pixel);
                // printf("colorA %x\n", color);
            }

            if (vg_draw_pixel(j, i, color)!=0) return 1;
            sprite++;
        }
    }
    return 0;
}


int (vg_clear)(){
    for (uint16_t i = 0; i < h_res; i++){
        for (uint16_t j = 0; j < v_res; j++){
            if (vg_draw_pixel(i, j, 0)!=0) return 1;
        }
    }
    return 0;
}

// ----------- SPRITE ---------------

// typedef struct {
//     int x, y; // current position
//     int width, height; // dimensions
//     int xspeed, yspeed; // current speed
//     char *map; // the pixmap
// } Sprite;

// //sprite.c 
// /** Creates a new sprite from XPM "pic", with specified
// * position (within the screen limits) and speed;
// * Does not draw the sprite on the screen
// * Returns NULL on invalid pixmap.
// */
// Sprite *create_sprite(const char *pic[], int x, int y,
//                 int xspeed, int yspeed) {
//     //allocate space for the "object"
//     Sprite *sp = (Sprite *) malloc ( sizeof(Sprite));
//     xpm_image_t img;
//     if( sp == NULL )
//         return NULL;
//     // read the sprite pixmap
//     sp->map = xpm_load(pic, XPM_INDEXED, &img);
//     if( sp->map == NULL ) {
//         free(sp);
//         return  NULL;
//     }
//     sp->width = img.width; sp->height=img.height;
//     //...
//     return sp;
// }

// void destroy_sprite(Sprite *sp) {
//     if( sp == NULL )
//         return;
//     if( sp ->map )
//         free(sp->map);
//     free(sp);
//     sp = NULL; // XXX: pointer is passed by value
//     // should do this @ the caller
// }
// // XXX: move_sprite would be a more appropriate name
// int animate_sprite(Sprite *sp) {
//     //...
// }
// /* Some useful non-visible functions */
// static int draw_sprite(Sprite *sp, char *base) {
//     //...
// }
// static int check_collision(Sprite *sp, char *base) {
//     //...
// }

// #include <stdarg.h> 

// typedef struct {
// Sprite *sp; // standard sprite
//     int aspeed; // no. frames per pixmap
//     int cur_aspeed; // no. frames left to next change
//     int num_fig; // number of pixmaps
//     int cur_fig; // current pixmap
//     char **map; // array of pointers to pixmaps
// } AnimSprite;

// AnimSprite *create_animSprite(uint8_t no_pic,
//             char *pic1[], ...) {
//     AnimSprite *asp = malloc(sizeof(AnimSprite));
//     // create a standard sprite with first pixmap
//     asp->sp = create_sprite(pic1,0,0,0,0);
//     // allocate array of pointers to pixmaps
//     asp->map = malloc((no_pic) * sizeof(char *));
//     // initialize the first pixmap
//     asp->map[0] = asp->sp->map;
//     // continues in next transparency
//     // initialize the remainder with the variable arguments
//     // iterate over the list of arguments
//     va_list ap;
//     va_start(ap, pic1);
//     for( i = 1; i <no_pic; i++ ) {
//         char **tmp = va_arg(ap, char **);
//         xpm_image_t img;
//         asp->map[i] = xpm_load(tmp, XPM_INDEXED, &img);
//         if( asp->map[i] == NULL || img.width != asp->sp->width || 
//                 img.height != asp->sp->height) {
//             // failure: release allocated memory
//             for(j = 1; j<i;j ++)
//                 free(asp->map[i]);
//             free(asp->map);
//             destroy_sprite(asp->sp);
//             free(asp);
//             va_end(ap);
//             return NULL;
//         }
//     }
//     va_end(ap);
//     //...
// }

// int animate_animSprite(AnimSprite *sp,);
// void destroy_animSprite(AnimSprite *sp);

// va_list to traverse the list 
// va_start to initialize the list
// va_start 2nd arg is the last known param
// va_arg get the next arg 
// va_arg 2nd arg is the type of that unnamed arg 
// va_end to finalize access

/*
int foo(int required, ...) {
va_list ap; // "pointer" to next argument
va_start(ap, required); // initializes ap to point to
// first (unnamed) argument of the list ;
// the 2nd argument of va_start is the last named argument
int i = va_arg(ap, int); // accesses the next list argument
// the second argument of va_arg() is the type
// on first call, returns the value of the first
// argument after the last fixed argument
float i = va_arg(ap, float);
char *s = va_arg(ap, char *);
va_end(ap); // must be called to finalize the access
}
*/


