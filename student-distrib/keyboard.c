#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

#define KEYBOARD_IRQ_NUM 1
#define KEYBOARD_PORT 0x60

// 0x60 is scan code

char key_map[128] = {
    '\0',' ','1','2','3','4','5','6','7','8','9','0','-','=',
    '\b','\t','q','w','e','r','t','y','u','i','o','p','[',
    ']','\n',' ','a','s','d','f','g','h','j','k','l',
    ';',' ',' ',' ',' ','z','x','c','v','b','n','m'
};

void keyboard_init(void){
    enable_irq(KEYBOARD_IRQ_NUM);
}


void keyboard_handler(){
    unsigned char scan_code;
    scan_code = inb(KEYBOARD_PORT);
    putc(key_map[scan_code]);
    send_eoi(KEYBOARD_IRQ_NUM);
}
