#ifndef _PAGING_H
#define _PAGING_H

extern void page_init(void);
extern void set_up_vidmap(void);
extern void set_up_pid_map(int pid);

#endif /* _PAGING_H */
