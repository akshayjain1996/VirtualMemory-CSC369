#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;

int *ref_bits;

int hand;

/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
	if(hand == memsize - 1){
		hand = 0;
	}

	if(ref_bits[hand] == 0){
		hand++;
		return hand - 1;
	}

	ref_bits[hand] = 0;
	hand++;

	return clock_evict();
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
	ref_bits[p -> frame >> PAGE_SHIFT] = 1;
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
	ref_bits = malloc(memsize * sizeof(int));
	int i = 0;
	for(i = 0; i < memsize; i++){
		ref_bits[i] = 0;
	}
	hand = 0;
}
