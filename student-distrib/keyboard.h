#ifndef _KEYBOARD_H
#define _KEYBOARD_H

extern void keyboard_init(void);
extern void keyboard_handler(void);
void kb_newline(char* buffer);
extern char* kb_buffer[128];
extern int count;
extern int clear_screen;
extern int enter_pressed;
extern int tab_flag;

#endif /* _KEYBOARD_H */
