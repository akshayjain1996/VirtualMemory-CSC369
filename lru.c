#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


extern int memsize;

extern int debug;

extern struct frame *coremap;


struct node{
	int frame;
	struct node* next;
};

struct node *head;

/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
	int ret_frame = head -> frame;
	struct node *temp = head;
	head = head -> next;
	free(temp);
	return ret_frame;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
	int frame = p -> frame >> PAGE_SHIFT;
	
	if(head == NULL){
		head = malloc(sizeof(struct node));
		head -> frame = frame;
		head -> next = NULL;
		return;
	}

	struct node *iter = head;

	while(iter -> next){
		if(iter -> next -> frame == frame){
			iter -> next = iter -> next -> next;
			if(iter -> next == NULL){
				break;
			}
		}
		iter = iter -> next;
	}

	iter -> next = malloc(sizeof(struct node));
	iter -> next -> frame = frame;
	iter -> next -> next = NULL;

}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
	head = NULL;

}
