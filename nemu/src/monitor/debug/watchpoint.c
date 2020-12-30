#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
/* get a free watchpoint from list free_ */
WP* new_wp() {
  if (free_ == NULL) return NULL;

  WP *newWP = free_;
  free_ = free_->next;
  newWP->next = head;
  head = newWP;

  return newWP;
}

/* get wp back to the list free_ */
void free_wp(WP *wp) {
  WP *temp = head;

  if (wp == head) {
    head = wp->next;
    wp->next = free_;
    free_ = wp;
    return;
  }
  while(temp->next != wp) temp = temp->next;
  temp->next = wp->next;
  wp->next = free_;
  free_ = wp;
}

/* print watchpoints */
void printWP() {
  if(head == NULL) {
		printf("No watchpoints.\n");
		return;
	}
	printf("No  EXPR                Value\n");
	WP *p = head;
	while(p != NULL) {
		printf("%-5d%-20s%#x\n", p->NO, p->expr, p->value);
		p = p -> next;
	}
}

