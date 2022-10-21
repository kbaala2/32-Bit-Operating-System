#ifndef _RTC_H
#define _RTC_H

extern void rtc_init(void);
extern void rtc_handler(void);
extern int rtc_open(void);
extern int rtc_read(void);
extern int rtc_write(uint8_t *freq);
extern int rtc_close(void);

#endif /* _RTC_H */
