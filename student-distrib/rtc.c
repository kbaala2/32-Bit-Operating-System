void rtc_init(void) {
    /* sets the registers for the rtc */
    cli(); 
    outb(0x70, 0x8A);
    sti();
    
}