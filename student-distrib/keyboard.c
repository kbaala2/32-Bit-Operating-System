#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

#define KEYBOARD_IRQ_NUM 1
#define KEYBOARD_PORT 0x60

// 0x60 is scan code

char key_map[256] = {
    '\0','\0','1','2',
    '3','4','5','6',
    '7','8','9','0',
    '-','=','\0','\0',
    'q','w','e','r',
    't','y','u','i',
    'o','p','[',']',
    '\n','\0','a','s',
    'd','f','g','h',
    'j','k','l',';',
    '\0','\0','\0','\0',
    'z','x','c','v',
    'b','n','m','\0',
    '\0','\0','\0','\0', //x34 first
    '\0',' ','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0', //x5C first
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0', //x80 first
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0',
    '\0','\0','\0','\0', //xB0 first
};

/* void keyboard_init(void);
 * Inputs: void
 * Return Value: none
 * Function: Initializes keyboard by enabling keyboard irq */
void keyboard_init(void){
    enable_irq(KEYBOARD_IRQ_NUM);
}

/* void keyboard_handler(void);
 * Inputs: void
 * Return Value: none
 * Function: handles keyboard interrupt */
void keyboard_handler(){
    unsigned char scan_code;
    scan_code = inb(KEYBOARD_PORT);
    if(scan_code <= 0x39){
        putc(key_map[scan_code]);
    }
    send_eoi(KEYBOARD_IRQ_NUM);
}
