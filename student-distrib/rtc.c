#include "rtc.h"
#include "lib.h"
#include "i8259.h"

void rtc_init(void) {
    /* sets the registers for the rtc */ 
    outb(0x0B, 0x70);
    char prev = inb(0x71);
    outb(0x0B, 0x70);
    outb(prev | 0x40, 0x71);
    enable_irq(8);
    outb(0x8A, 0x70);
    outb(0x06, 0x71);
}

void rtc_handler() {
    outb(0x0C, 0x70);
    inb(0x71);
    //test_interrupts();
    send_eoi(8);
}
