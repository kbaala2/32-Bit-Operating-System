
#include "lib.h"
#include "i8259.h"
#include "rtc.h"

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
    char prev = inb(0x71); //read value at register B
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
    BLOCK_FLAG = 1;
    send_eoi(8);
    
}

/* void rtc_open(void);
 * Inputs: void
 * Return Value: none
 * Function: resets rtc to 2hz*/
int rtc_open(const uint8_t* filename){
    
    outb(0x8A, 0x70);		// set index to register A, disable NMI
    char prev=inb(0x71);	// get initial value of register A
    outb(0x8A, 0x70);		// reset index to A
    outb((prev & 0xF0) | 0x0F, 0x71); //write only our rate to A. Note, rate is 0x0F, which sets rate to 2hz
    
    return 0;
}

/* void rtc_read(int32_t fd, void *buf, int32_t nbytes);
 * Inputs: int32_t fd - file data
           void *buf - buffer that hold frequency
           int32_t nbytes - N/A
 * Return Value: none
 * Function: blocks until next interrupt*/
int rtc_read(int32_t fd, void *buf, int32_t nbytes){
    while(!BLOCK_FLAG);
    BLOCK_FLAG = 0;
    outb(0x0C, 0x70);	// select register C
    inb(0x71);		// just throw away contents
    return 0; 
}

/* int rtc_write(int32_t fd, const void *buf, int32_t nbytes)
 * Inputs: int32_t fd - file data
 *         const void *buf - buffer that holds frequency
 *         in32_t nbytes - N/A
 * Return Value: none
 * Function: writes new frequency from the buffer to the RTC 
 */ 
int rtc_write(int32_t fd, const void *buf, int32_t nbytes){
    int target;
    int rate;
    int new_rate;
    int max_freq = 32768; //max frequency value
    int* result = (int*)buf;
    int freq = result[0];

    if(freq == NULL || ((freq & (freq - 1)) != 0)){ // check if freq is a power of 2 and is not null 
        return -1;
    }

    /*check for each rate if the target frequency is equal to the new frequency, then set the new rate*/
    for(rate = 6; rate < 17; rate++){
        target = max_freq >> (rate - 1);
        /*set new rate*/
        if(target == freq){
            new_rate = rate - 1;
            break;
        }
    }
    /*check if new rate is within bounds*/
    if(new_rate < 6){
        new_rate = 6;
    }

    /*set new frequency*/
    outb(0x8A, 0x70);		// set index to register A, disable NMI
    char prev=inb(0x71);	// get initial value of register A
    outb(0x8A, 0x70);		// reset index to A
    outb((prev & 0xF0) | new_rate, 0x71); //write only our rate to A. Note, rate is 0x0F, which sets rate to freq

    return nbytes;
}

/* void rtc_cclose(int32_t fd);
 * Inputs: int32_t fd - file data
 * Return Value: none
 * Function: does nothing*/
int rtc_close(int32_t fd){
    return 0;
}
