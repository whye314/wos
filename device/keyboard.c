#include "keyboard.h"
#include "console.h"
#include "io.h"
#include "interrupt.h"
#include "print.h"


#define KBD_BUF_PORT 0x60

#define ESC '\033'
#define BACKSPACE '\b'
#define TAB '\t'
#define ENTER '\r'
#define DELETE '\177'

#define CHAR_INVISIBLE 0
#define CTRL_L_CHAR CHAR_INVISIBLE
#define CTRL_R_CHAR CHAR_INVISIBLE
#define SHIFT_L_CHAR CHAR_INVISIBLE
#define SHIFT_R_CHAR CHAR_INVISIBLE
#define ALT_L_CHAR CHAR_INVISIBLE
#define ALT_R_CHAR CHAR_INVISIBLE
#define CAPS_LOCK_CHAR CHAR_INVISIBLE

#define shift_l_make 0x2a
#define shift_r_make 0x36
#define alt_l_make 0x38
#define alt_r_make 0xe038
#define ctrl_l_make 0x1d
#define ctrl_r_make 0xe01d
#define caps_lock_make 0x3a

static uint8_t ctrl_status, shift_status, alt_status, caps_lock_status, ext_scancode;

static char key_map[][2] = {
    {0, 0},
    {ESC, ESC},
    {'1', '!'},
    {'2', '@'},
    {'3', '#'},
    {'4', '$'},
    {'5', '%'},
    {'6', '^'},
    {'7', '&'},
    {'8', '*'},
    {'9', '('},
    {'0', ')'},
    {'-', '_'},
    {'=', '+'},
    {BACKSPACE, BACKSPACE},
    {TAB, TAB},
    {'q', 'Q'},
    {'w', 'W'},
    {'e', 'E'},
    {'r', 'R'},
    {'t', 'T'},
    {'y', 'Y'},
    {'u', 'U'},
    {'i', 'I'},
    {'o', 'O'},
    {'p', 'P'},
    {'[', '{'},
    {']', '}'},
    {ENTER, ENTER},
    {CTRL_L_CHAR, CTRL_L_CHAR},
    {'a', 'A'},
    {'s', 'S'},
    {'d', 'D'},
    {'f', 'F'},
    {'g', 'G'},
    {'h', 'H'},
    {'j', 'J'},
    {'k', 'K'},
    {'l', 'L'},
    {';', ':'},
    {'\'', '"'},
    {'`', '~'},
    {SHIFT_L_CHAR, SHIFT_L_CHAR},
    {'\\', '|'},
    {'z', 'Z'},
    {'x', 'X'},
    {'c', 'C'},
    {'v', 'V'},
    {'b', 'B'},
    {'n', 'N'},
    {'m', 'M'},
    {',', '<'},
    {'.', '>'},
    {'/', '?'},
    {SHIFT_R_CHAR, SHIFT_R_CHAR},
    {'*', '*'},
    {ALT_L_CHAR, ALT_L_CHAR},
    {' ', ' '},
    {CAPS_LOCK_CHAR, CAPS_LOCK_CHAR}
};






static void int_keyboard_handler(){

    uint16_t scancode = inb(KBD_BUF_PORT);
    if(scancode == 0xe0){
        ext_scancode = 1;
        return;
    }
    if(ext_scancode){
        scancode = 0xe000 + scancode;
        ext_scancode = 0;
    }

    uint8_t break_code = (scancode & 0x80 != 0);
    scancode = scancode & (~0x80);
    if(break_code){
        if(scancode == ctrl_l_make || scancode == ctrl_r_make){
            ctrl_status = 0;
        }
        else if(scancode == shift_l_make || scancode == shift_r_make){
            shift_status = 0;
        }
        else if(scancode == alt_l_make || scancode == alt_r_make){
            alt_status = 0;
        }
        return;
    }
    else if(scancode < 0x3b || scancode == alt_r_make || scancode == ctrl_r_make){
        uint8_t shift = 0;
        if((scancode < 0x10) || (scancode == 0x1a) || \
            (scancode == 0x27) || (scancode == 0x2b) || \
            (scancode == 0x28) || (scancode == 0x1c) || \
            (scancode == 0x1b) || (scancode == 0x33) || \
            (scancode == 0x34) || (scancode == 0x35)){
                shift = shift_status;
        }else{
            shift = shift_status ^ caps_lock_status;
        }
            uint8_t index = scancode & 0xff;
            char c = key_map[index][shift];
            if(c){
                put_char(c);
                return;
            }
        if((scancode == shift_l_make) || (scancode == shift_r_make)){
            shift_status = 1;
        }else if((scancode == ctrl_l_make) || (scancode == ctrl_r_make)){
            ctrl_status = 1;
        }else if((scancode == alt_l_make) || (scancode == alt_r_make)){
            alt_status = 1;
        }else if(scancode == caps_lock_make){
            caps_lock_status = !caps_lock_status;
        }
        else{
            put_str("unknown key.\n");
        }
    }

}

void keyboard_init(){
    console_put_str("keyboard initization start.\n");
    register_handler(0x21, int_keyboard_handler);
    console_put_str("keyboard initization finish.\n");
}