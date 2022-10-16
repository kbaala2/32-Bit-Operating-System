#include "paging.h"
#include "x86_desc.h"
#include "lib.h"

#define VIDEO       0xB8000
#define VIDEO_MEM_INDEX 184
#define NUM_ENTRIES 1024
#define START_OF_KERNEL 0x400000
extern void loadPageDirectory(uint32_t* page_dir);
extern void enablePaging();

page_directory_entry_t pde[NUM_ENTRIES] __attribute__((aligned(4096))); //aligned every 4mb
page_table_entry_t pte[NUM_ENTRIES] __attribute__((aligned(4096))); //aligned every 4kb

void page_init(){
    int i;
    /* generically fill Page Table */
    for(i = 0; i < NUM_ENTRIES; i++){
        pte[i].present = 0;
        pte[i].read_write = 1;
        pte[i].user_supervisor = 0;
        pte[i].write_through = 0;
        pte[i].cache_disable = 0;
        pte[i].accessed = 0;
        pte[i].dirty = 0;
        pte[i].page_attribute_table = 0;
        pte[i].global = 0;
        pte[i].available = 0x00;
        pte[i].page_base_addr = 0x00000;
    }
    /* fill video memory page in page table */
    pte[VIDEO_MEM_INDEX].page_base_addr = (uint32_t) VIDEO >> 12; //shift from 32 bits to 20 bits
    pte[VIDEO_MEM_INDEX].read_write = 0;
    pte[VIDEO_MEM_INDEX].user_supervisor = 0;
    pte[VIDEO_MEM_INDEX].write_through = 0;
    pte[VIDEO_MEM_INDEX].cache_disable = 0;
    pte[VIDEO_MEM_INDEX].accessed = 0;
    pte[VIDEO_MEM_INDEX].dirty = 0;
    pte[VIDEO_MEM_INDEX].page_attribute_table = 0;
    pte[VIDEO_MEM_INDEX].global = 0;
    pte[VIDEO_MEM_INDEX].available = 0x00;
    pte[VIDEO_MEM_INDEX].present = 1;

    /* generically fill Page Directory */
    for(i = 0; i < NUM_ENTRIES; i++){
        pde[i].present = 0;
        pde[i].read_write = 1;
        pde[i].user_supervisor = 0;
        pde[i].write_through = 0;
        pde[i].cache_disable = 0;
        pde[i].accessed = 0;
        pde[i].dirty = 0;
        pde[i].page_size = 0;
        pde[i].global = 0;
        pde[i].available = 0x00;
        pde[i].page_table_base_addr = 0x00000;
    }
    /* fill video memory table in page directory */
    pde[0].page_table_base_addr = ((uint32_t) pte) >> 12; //shift from 32 bits to 20 bits
    pde[0].read_write = 1;
    pde[0].user_supervisor = 0;
    pde[0].write_through = 0;
    pde[0].cache_disable = 0;
    pde[0].accessed = 0;
    pde[0].dirty = 0;
    pde[0].page_size = 0;
    pde[0].global = 0;
    pde[0].available = 0x00;
    pde[0].present = 1;

    /* fill kernel page in page directory */
    pde[1].page_table_base_addr = ((uint32_t) START_OF_KERNEL) >> 12; //shift from 32 bits to 20 bits
    pde[1].read_write = 1;
    pde[1].user_supervisor = 0;
    pde[1].write_through = 0;
    pde[1].cache_disable = 0;
    pde[1].accessed = 0;
    pde[1].dirty = 0;
    pde[1].page_size = 1;
    pde[1].global = 0;
    pde[1].available = 0x00;
    pde[1].present = 1;

    /* load page directory and enable paging for 4MB page and 4kB page*/
    loadPageDirectory(pde);
    enablePaging();
}
