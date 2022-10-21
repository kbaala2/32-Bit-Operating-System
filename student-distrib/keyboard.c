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

char key_map[256] = {
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

    else if(scan_code == 0x1C){
        enter_pressed = 1;
    }
    
    if((key_map[scan_code] >= 32 && key_map[scan_code] <= 127) || (key_map[scan_code] >= 8 && key_map[scan_code] <= 10)){

        //ctrl + l
        if(ctrl_flag == 1 && scan_code == 0x26){
            //clear_screen = 1;
            clear();
            clear_pos();
            for(i = 0; i < count; i++){
                kb_buffer[i] = '\0';
            }
            count = 0;
        }
        else if(count < 127){
            if(scan_code == 0x0F){
                kb_buffer[count] = key_map[scan_code];
                count++;
                putc(' ');
                putc(' ');
                putc(' ');
                putc(' ');
            }
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
                if(shift_flag ^ caps_flag){
                    switch((int)key_map[scan_code]){
                        case 96: shifted_char = 126;
                            break;
                        case 49: shifted_char = 33;
                            break;
                        case 50: shifted_char = 64;
                            break;
                        case 51: shifted_char = 35;
                            break;
                        case 52: shifted_char = 36;
                            break;
                        case 53: shifted_char = 37;
                            break;
                        case 54: shifted_char = 94;
                            break;
                        case 55: shifted_char = 38;
                            break;
                        case 56: shifted_char = 42;
                            break;
                        case 57: shifted_char = 40;
                            break;
                        case 48: shifted_char = 41;
                            break;
                        case 45: shifted_char = 95;
                            break;
                        case 61: shifted_char = 43;
                            break;
                        case 91: shifted_char = 123;
                            break;
                        case 93: shifted_char = 125;
                            break;
                        case 92: shifted_char = 124;
                            break;
                        case 59: shifted_char = 58;
                            break;
                        case 39: shifted_char = 34;
                            break;
                        case 44: shifted_char = 60;
                            break;
                        case 46: shifted_char = 62;
                            break;
                        case 47: shifted_char = 63;
                            break;
                        case 10: shifted_char = 10;
                            break;
                        default: shifted_char = (key_map[scan_code] - 32);
                            break;
                    }
                    putc((char)shifted_char);
                    kb_buffer[count] = (char)shifted_char;
                    count++;
                }
                else{
                    putc(key_map[scan_code]);
                    kb_buffer[count] = key_map[scan_code];
                    count++;
                }
            }
            // enter_pressed = 0;
        }
        else if(count == 127 && scan_code == 0x1C){
            // enter_pressed = 1;
            putc(key_map[scan_code]);
            kb_buffer[count] = key_map[scan_code];
            count = 0;
        }
    }
    send_eoi(KEYBOARD_IRQ_NUM);
    sti();
}
