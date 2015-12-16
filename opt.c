#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"

extern int memsize;

extern int debug;

extern char *tracefile;

extern struct frame *coremap;

FILE *infp = NULL;

struct node{
	addr_t vaddr;
	struct node* next;
};

struct node *head;
struct node *last;

unsigned long *frame_map;

void read_file();
void handle_next_frame(addr_t vaddr);

/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {

	int i = 0, counter = 0, max = 0;
	int ret_frame = 0;

	for(i = 0; i < memsize; i++){
		counter = 0;
		if(frame_map[i] != 0){
			struct node *iter = head;
			while(iter) {
				counter++;
				if(iter -> vaddr == frame_map[i]) {
					if(counter >= max){
						max = counter;
						ret_frame = i;
					}
					break;
				}
				iter = iter -> next;
			}
			if(iter == NULL){
				return i;
			}
		}
	}
	return ret_frame;


}

 
void opt_ref(pgtbl_entry_t *p) {
	frame_map[p -> frame >> PAGE_SHIFT] = head -> vaddr;
	head = head -> next;
	
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	frame_map = malloc(memsize * sizeof(unsigned long));
	int i = 0;
	for(i = 0; i < memsize; i++){
		frame_map[i] = 0;
	}

	head = NULL;
	last = NULL;
	read_file();
}

void read_file() {

	if((infp = fopen(tracefile, "r")) == NULL) {
		perror("Error opening tracefile:");
		exit(1);
	}

	char buf[256];
	addr_t vaddr = 0;
	char type;
	
	while(fgets(buf, 256, infp) != NULL) {
		if(buf[0] != '=') {
			sscanf(buf, "%c %lx", &type, &vaddr);
			handle_next_frame(vaddr);
		} 
	}
}


void handle_next_frame(addr_t vaddr){
	struct node *new_node = malloc(sizeof(struct node));
	new_node -> vaddr = vaddr;
	new_node -> next = NULL;
	if(head == NULL){
		head = new_node;
		last = head;
	} else {
		last -> next = new_node;
		last = last -> next;
	}

}
