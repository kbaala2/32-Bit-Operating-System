#include "rtc.h"
#include "lib.h"
#include "i8259.h"

//FLAGS
volatile int BLOCK_FLAG = 0; //flag to tell read to block until next interrupt


// unsigned int Log2n(unsigned int n);


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

/* void rtc_open(void);
 * Inputs: void
 * Return Value: none
 * Function: resets rtc to 2hz*/
int rtc_open(){
    
    outb(0x8A, 0x70);		// set index to register A, disable NMI
    char prev=inb(0x71);	// get initial value of register A
    outb(0x8A, 0x70);		// reset index to A
    outb((prev & 0xF0) | 0x0F, 0x71); //write only our rate to A. Note, rate is 0x0F, which sets rate to 2hz
    
    return 0;
}


// unsigned int Log2n(unsigned int n)
// {
//     return (n > 1) ? 1 + Log2n(n / 2) : 0;
// }

int rtc_read(){
    while(BLOCK_FLAG);
    outb(0x0C, 0x70);	// select register C
    inb(0x71);		// just throw away contents
    return 0; 
}

int rtc_write(uint8_t *freq){
    int target;
    int rate;
    int new_rate;
    int max_freq = 32768;
    
    if(freq == NULL || (freq & (freq - 1) != 0)){
        return -1;
    }

    for(rate = 3; rate  < 17; rate++){
        target = max_freq >> (rate - 1);
        if(target == freq){
            new_rate = rate - 1;
            break;
        }
    }

    outb(0x8A, 0x70);		// set index to register A, disable NMI
    char prev=inb(0x71);	// get initial value of register A
    outb(0x8A, 0x70);		// reset index to A
    outb((prev & 0xF0) | new_rate, 0x71); //write only our rate to A. Note, rate is 0x0F, which sets rate to freq

    return 0;
}