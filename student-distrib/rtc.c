#include "rtc.h"
#include "lib.h"
#include "i8259.h"

void rtc_init(void) {
    /* sets the registers for the rtc */
    cli(); 
    outb(0x70, 0x8A);
    sti();
    
}

void rtc_handler() {

}
