#include "keyboard.h"
#include "lib.h"

// 0x60 is scan code

char key_map[128] = {
    0,'','1','2','3','4','5','6','7','8','9','0','-','=',
    '\b','\t','q','w','e','r','t','y','u','o','p','[',
    ']','','','a','s','d','f','g','h','j','k','l',
    '','','z','x','c','v','b','n','m','','','',
}

void keyboard_init(void){
    
}


void keyboard_handler();