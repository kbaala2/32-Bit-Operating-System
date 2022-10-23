#include "keyboard.h"
#include "lib.h"
#include "i8259.h"

#define KEYBOARD_IRQ_NUM 1
#define KEYBOARD_PORT 0x60

int shift_flag = 0;
int ctrl_flag = 0;
int caps_flag = 0;
char* kb_buffer[128];
int count = 0;
int clear_screen = 0;
int enter_pressed = 0;
int tab_flag = 0;
int shifted_char;

// 0x60 is scan code

signed char key_map[256] = {
    '\0','\0','1','2',
    '3','4','5','6',
    '7','8','9','0',
    '-','=','\b','\t',
    'q','w','e','r',
    't','y','u','i',
    'o','p','[',']',
    '\n','\0','a','s',
    'd','f','g','h',
    'j','k','l',';',
    '\'','`','\0','\\',
    'z','x','c','v',
    'b','n','m',',',
    '.','/','\0','\0', //x34 first
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
void keyboard_handler(void){
    cli();
    unsigned char scan_code;
    int i;
    scan_code = inb(KEYBOARD_PORT);

    //control presed
    if(scan_code == 0x1D){
        ctrl_flag = 1;
    }

    //shift pressed
    else if(scan_code == 0x2A){
        shift_flag = 1;
    }  

    //turn caps on
    else if(caps_flag == 0 && scan_code == 0xBA){
        caps_flag = 1;
    }

    //turn caps off
    else if(caps_flag == 1 && scan_code == 0xBA){
        caps_flag = 0;
    }

    //shift released
    else if(scan_code == 0xAA){
        shift_flag = 0;
    }

    //ctrl released
    else if(scan_code == 0x9D){
        ctrl_flag = 0;
    }
    //enter pressed
    else if(scan_code == 0x1C){
        enter_pressed = 1;
    }
    
    /*check if valid input (is alphanumeric character or enter, tab, backspace). Range for alphanumeric characters in ASCII table is between
    32-127, and range for enter, tab, backspace is between 8 and 10*/
    if((key_map[scan_code] >= 32 && key_map[scan_code] <= 127) || (key_map[scan_code] >= 8 && key_map[scan_code] <= 10)){

        //ctrl + l, scan_code for l is 0x26
        if(ctrl_flag == 1 && scan_code == 0x26){
            //clear_screen = 1;
            clear();
            clear_pos();
            for(i = 0; i < count; i++){
                kb_buffer[i] = '\0'; //reset everything in keyboard buffer to null
            }
            count = 0; //reset count
        }

        //if there's still space in the buffer. Max amount of chars in kb_buffer is 128
        else if(count < 127){
            //check if scan_code is equal to scan_code for tab
            if(scan_code == 0x0F){ 
                kb_buffer[count] = key_map[scan_code];
                count++;
                putc(' ');
                putc(' ');
                putc(' ');
                putc(' ');
            }
            //check if scan_code is equal to scan_code for backspace
            else if(scan_code == 0x0E){
                if(count > 0){
                    if(kb_buffer[count-1] == '\t'){
                        tab_flag = 1;
                    }
                    putc(key_map[scan_code]);
                    count--;
                    kb_buffer[count] = '\0';
                }
            }
            else{
                //check if either shift or caps is high but not both
                if(shift_flag ^ caps_flag){
                    //checks which char that might need to be shifted up is
                    switch((int)key_map[scan_code]){
                        /* each case represents the ascii value of the char that we are checking to see 
                            if it should be shifted or not. if caps flag is low than we to use the shifted ascii value*/
                        //case for ` -> ~
                        case 96: 
                            if(caps_flag){
                                shifted_char = 96;
                            }
                            else{
                                shifted_char = 126;
                            }
                            break;
                        //case for 1 -> !
                        case 49:
                            if(caps_flag){
                                shifted_char = 49;
                            }
                            else{
                                shifted_char = 33;
                            }
                            break;
                        //case for 2 -> @
                        case 50:
                            if(caps_flag){
                                shifted_char = 50;
                            }
                            else{
                                shifted_char = 64;
                            }
                            break;
                        // case for 3 -> #
                        case 51:
                            if(caps_flag){
                                shifted_char = 51;
                            }
                            else{
                                shifted_char = 35;
                            }
                            break;
                        // case for 4 -> $
                        case 52:
                            if(caps_flag){
                                shifted_char = 52;
                            }
                            else{
                                shifted_char = 36;
                            }
                            break;
                        // case for 5 -> %
                        case 53:
                            if(caps_flag){
                                shifted_char = 53;
                            }
                            else{
                                shifted_char = 37;
                            }
                            break;
                        // case for 6 -> ^
                        case 54:
                            if(caps_flag){
                                shifted_char = 54;
                            }
                            else{
                                shifted_char = 94;
                            }
                            break;
                        // case for 7 -> &
                        case 55:
                            if(caps_flag){
                                shifted_char = 55;
                            }
                            else{
                                shifted_char = 38;
                            }
                            break;
                        // case for 8 -> *
                        case 56:
                            if(caps_flag){
                                shifted_char = 56;
                            }
                            else{
                                shifted_char = 42;
                            }
                            break;
                        // case for 9 -> (
                        case 57:
                            if(caps_flag){
                                shifted_char = 57;
                            }
                            else{
                                shifted_char = 40;
                            }
                            break;
                        // case for 0 -> )
                        case 48:
                            if(caps_flag){
                                shifted_char = 48;
                            }
                            else{
                                shifted_char = 41;
                            }
                            break;
                        // case for - -> _
                        case 45:
                            if(caps_flag){
                                shifted_char = 45;
                            }
                            else{
                                shifted_char = 95;
                            }
                            break;
                        // case for = -> +
                        case 61:
                            if(caps_flag){
                                shifted_char = 61;
                            }
                            else{
                                shifted_char = 43;
                            }
                            break;
                        // case for [ -> {
                        case 91:
                            if(caps_flag){
                                shifted_char = 91;
                            }
                            else{
                                shifted_char = 123;
                            }
                            break;
                        // case for ] -> }
                        case 93:
                            if(caps_flag){
                                shifted_char = 93;
                            }
                            else{
                                shifted_char = 125;
                            }
                            break;
                        // case for \ -> |
                        case 92:
                            if(caps_flag){
                                shifted_char = 92;
                            }
                            else{
                                shifted_char = 124;
                            }
                            break;
                        // case for ; -> :
                        case 59:
                            if(caps_flag){
                                shifted_char = 59;
                            }
                            else{
                                shifted_char = 58;
                            }
                            break;
                        // case for ' -> "
                        case 39:
                            if(caps_flag){
                                shifted_char = 39;
                            }
                            else{
                                shifted_char = 34;
                            }
                            break;
                        // case for , -> <
                        case 44:
                            if(caps_flag){
                                shifted_char = 44;
                            }
                            else{
                                shifted_char = 60;
                            }
                            break;
                        // case for . -> >
                        case 46:
                            if(caps_flag){
                                shifted_char = 46;
                            }
                            else{
                                shifted_char = 62;
                            }
                            break;
                        // case for / -> ?
                        case 47:
                            if(caps_flag){
                                shifted_char = 47;
                            }
                            else{
                                shifted_char = 63;
                            }
                            break;
                        // case for enter
                        case 10:
                            if(caps_flag){
                                shifted_char = 10;
                            }
                            else{
                                shifted_char = 10;
                            }
                            break;
                        /* default case for alphabetic chacarters. have to subtract 32
                           since the difference in ascii table between lowercase and uppercase */
                        default: shifted_char = (key_map[scan_code] - 32);
                            break;
                    }
                    putc((char)shifted_char);
                    kb_buffer[count] = (char)shifted_char;
                    count++;
                }
                // no shift or caps lock enabled
                else{
                    putc(key_map[scan_code]);
                    kb_buffer[count] = key_map[scan_code];
                    count++;
                }
            }
            // enter_pressed = 0;
        }
        // if count is at kb_buffer limit (only room for enter) and scan_code is scan code for enter (0x1C)
        else if(count == 127 && scan_code == 0x1C){
            // enter_pressed = 1;
            putc(key_map[scan_code]);
            kb_buffer[count] = key_map[scan_code];
            count = 0; //reset count
        }
    }
    send_eoi(KEYBOARD_IRQ_NUM);
    sti();
}
