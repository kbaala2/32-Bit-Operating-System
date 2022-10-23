#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $8");
}

/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here

int div_zero_test(){
	TEST_HEADER;
	int j;
	int i = 1;
	j = i/0;
	return FAIL;
}

int paging_test(){
	TEST_HEADER;
	int c;
	unsigned int i = 3;
	int* j = &i;
	c = *j;
	return PASS;
}

int paging_test_2(){
	TEST_HEADER;
	int i;
	int* j = NULL;
	i = *j;
	return FAIL;
}

int cat_test(uint8_t* filename) {
	uint8_t i, filename, nbytes;
	char buf[100];

	nbytes = 100;

	if(open_f(filename) == -1){
		printf("bad filename");
		return FAIL;
	}

	for(i = 0; i < nbytes; i++){
		while(read_f(filename,buf,nbytes)) {
			printf("%c", buf[i]);
		}
	}

	close_f(filename);
	 
	return PASS;



}

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	//TEST_OUTPUT("page test", paging_test());
	//TEST_OUTPUT("page test 2", paging_test_2());
	//TEST_OUTPUT("div_zero_test", div_zero_test());
	while(1);
	// launch your tests here
	
}
