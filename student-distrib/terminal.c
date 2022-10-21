#include "keyboard.h"
#include "lib.h"
#include "terminal.h"

int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes){
    int i = 0;
    //cli();
    if(buf == NULL) {
        //sti();
        return -1;
    }
    if(nbytes <= 0) {
        //sti();  
        return 0;
    }
    if(nbytes > 128){
        nbytes = 128;
    }
    while(!enter_pressed){}
    char* result = (char*)buf;
    for(i = 0; i < nbytes; i++){
        result[i] = kb_buffer[i];
        kb_buffer[i] = '\0'; 
        if(result[i] == '\n') {
            nbytes = i + 1;
            break;
        }
    }
    enter_pressed = 0;
    count = 0;
    //sti();
    return nbytes;
}

int32_t terminal_write (int32_t fd, const void* buf, int32_t nbytes){
    cli();
    if(buf == NULL) {
        sti();
        return -1;
    }
    if(nbytes <= 0) {
        sti();
        return 0;
    }
    char* result = (char*)buf;
    // if(clear_screen){
    //     clear_screen = 0;
    //     clear();
    //     clear_pos();
    // }
    int i = 0;
    for(i = 0; i < nbytes; i++){
        putc(result[i]);
    }
    sti();
    return nbytes;
}

int32_t terminal_open (const uint8_t* filename){
    return 0;
}

int32_t terminal_close (int32_t fd){
    return 0;
}