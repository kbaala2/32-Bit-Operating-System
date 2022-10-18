#include "rtc.h"
#include "lib.h"
#include "i8259.h"

/* void rtc_init(void);
 * Inputs: void
 * Return Value: none
 * Function: Initializes rtc */
void rtc_init(void) {
    /* sets the registers for the rtc */ 
    outb(0x0B, 0x70); //Select register B
    char prev = inb(0x71); //read valye at register B
    outb(0x0B, 0x70); //set the index again
    outb(prev | 0x40, 0x71);  //turn on bit 6 of reigster B
    enable_irq(8); //RTC at irq 8
    outb(0x8A, 0x70); //disable NMI
    outb(0x06, 0x71); //set frequency to 1024
}


/* void rtc_handler(void);
 * Inputs: void
 * Return Value: none
 * Function: Handles rtc interrupt and sends EOI*/
void rtc_handler() {
    outb(0x0C, 0x70); //select register C
    inb(0x71); //throw away contents
    //test_interrupts();
    send_eoi(8);
}
