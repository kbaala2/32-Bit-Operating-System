#include "keyboard.h"
#include "lib.h"
#include "terminal.h"

#define MAX_BUFFER_SIZE 128

/* int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
 * Inputs: int32_t fd - file data
           void* buf - Buffer that will copy the keyboard buffer
           int32_t nbytes - number of bytes to be read
 * Return Value: number of bytes that were read
 * Function: when enter is pressed, reads the data from the keyboard buffer into the general buffer */
int32_t terminal_read (int32_t fd, void* buf, int32_t nbytes){
    int i = 0;
    if(buf == NULL) {   //return -1 if buffer is null
        return -1;
    }
    if(nbytes <= 0) {   //return 0 if no bytes are to be read
        return 0;
    }
    if(nbytes > MAX_BUFFER_SIZE){
        nbytes = MAX_BUFFER_SIZE;   //limit the amount of bytes to be read to max buffer size (128 chars)
    }

    while(!enter_pressed[visible_term]){} //do nothing if enter is not pressed

    memset(term_buffer[visible_term], 0, sizeof(term_buffer[visible_term]));
    memcpy(term_buffer[visible_term], kb_buffer[visible_term], sizeof(kb_buffer[visible_term]));
    memset(kb_buffer[visible_term], 0, sizeof(kb_buffer[visible_term]));

    char* result = (char*)buf;
    int act_term = get_act_terminal();
    
    for(i = 0; i < nbytes; i++){    //loop through each character in keyboard buffer once enter is pressed
        result[i] = term_buffer[act_term][i];   //copy contents to general buffer
      //  kb_buffer[act_term][i] = '\0';    //clear contents of keyboard buffer
        if(result[i] == '\n') {
            nbytes = i + 1; //when we reach the newline character, set nbytes to the amount of bytes read at that point
            break;
        }
    }
    enter_pressed[act_term] = 0;  //reset enter flag and index for keyboard buffer
    count[visible_term] = 0;
    return nbytes;  //return number of bytes read
}

/* int32_t terminal_write(int32_t fd, void* buf, int32_t nbytes)
 * Inputs: int32_t fd - file data
           void* buf - Buffer that will echo to the screen
           int32_t nbytes - number of bytes to be written
 * Return Value: number of bytes that were written
 * Function: writes from the buffer to the screen whenever it is populated */
int32_t terminal_write (int32_t fd, const void* buf, int32_t nbytes){
    cli();  //disable interrupts
    if(buf == NULL) {
        sti();
        return -1;  //checks if buffer is null
    }
    if(nbytes <= 0) {
        sti();
        return 0;   //return 0 if no bytes are to be written
    }
    char* result = (char*)buf;
    int i = 0;
    int act_term = get_act_terminal();
    for(i = 0; i < nbytes; i++){    //loop through each character in the buffer and output to screen
        if(result[i] == '\0') {
            continue;
        }
        term_buffer[act_term][i] = result[i];

        putc(term_buffer[act_term][i]);
    }
    sti();  //enable interrupts
    return nbytes;  //return number of bytes written
}

/* int32_t terminal_open(const uint8_t* filename)
 * Inputs: const uint8_t* filename - filename to open
 * Return Value: nothing
 * Function: initialize terminal stuff or do nothing */
int32_t terminal_open (const uint8_t* filename){
    return 0;
}

/* int32_t terminal_close(int32_t fd)
 * Inputs: int32_t fd - file data
 * Return Value: nothing
 * Function: clear terminal stuff or do nothing */
int32_t terminal_close (int32_t fd){
    return 0;
}
