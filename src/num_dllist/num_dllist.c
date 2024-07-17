#include "num_dllist.h"
#include "../exit_codes.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

num_dllist_t *dllist_init(uint8_t *exit_code) {
  num_dllist_t *dllist = (num_dllist_t *)malloc(sizeof(num_dllist_t));

  if (dllist == NULL) {
    fprintf(stderr, "Cannot initialize double linked list\n");
    *exit_code = other_errors;
    return NULL;
  }

  dllist->first = NULL;
  dllist->last = NULL;
  dllist->sign = SIGN_NOT_SET;
  dllist->size = 0;
  dllist->capacity = DLLIST_CAPACITY;

  return dllist;
}

void dllist_clear(num_dllist_t *dllist, uint8_t *exit_code) {
  num_node_t *temp;

  if (dllist == NULL) {
    fprintf(stderr, "Cannot clear double linked list\n");
    *exit_code = other_errors;
  }

  while (dllist->first != NULL) {
    temp = dllist->first;
    dllist->first = temp->next;
    free(temp);
  }

  dllist->first = NULL;
  dllist->last = NULL;
  dllist->sign = SIGN_NOT_SET;
  dllist->size = 0;
  dllist->capacity = 0;

  free(dllist);
}

void dllist_add_start(num_dllist_t *dllist, uint64_t element,
                      uint8_t *exit_code) {
  num_node_t *node = (num_node_t *)malloc(sizeof(num_node_t));

  if ((node == NULL) || (dllist == NULL)) {
    fprintf(stderr, "Cannot add element to the start of double linked list\n");
    *exit_code = other_errors;
  }

  if (dllist->size == dllist->capacity) {
    fprintf(stderr, "Cannot add element to the start of double linked list "
                    "because it is full\n");
    *exit_code = other_errors;
  }

  node->digit = element;
  node->next = dllist->first;
  node->prev = NULL;

  if (dllist->first)
    dllist->first->prev = node;
  dllist->first = node;

  if (dllist->last == NULL)
    dllist->last = node;

  ++dllist->size;
}

void dllist_add_end(num_dllist_t *dllist, uint64_t element,
                    uint8_t *exit_code) {
  num_node_t *node = (num_node_t *)malloc(sizeof(num_node_t));

  if ((node == NULL) || (dllist == NULL)) {
    fprintf(stderr,
            "Cannot add element to the end of the double linked list\n");
    *exit_code = other_errors;
  }

  if (dllist->size == dllist->capacity) {
    fprintf(stderr, "Cannot add element to the end of the double linked list "
                    "because it is full\n");
    *exit_code = other_errors;
  }

  node->digit = element;
  node->next = NULL;
  node->prev = dllist->last;

  if (dllist->last)
    dllist->last->next = node;
  dllist->last = node;

  if (dllist->first == NULL)
    dllist->first = node;

  ++dllist->size;
}

void dllist_print(num_dllist_t *dllist, uint8_t *exit_code) {
  num_node_t *temp = dllist->first;
  bool flag = false;

  if (dllist == NULL) {
    fprintf(stderr, "Cannot print double linked list\n");
    *exit_code = other_errors;
  }

  if (dllist->sign == -1)
    printf("-");

  while (temp) {
    if (flag) {
      printf("%09lu", temp->digit);
    } else {
      printf("%ld", temp->digit);
      flag = true;
    }
    temp = temp->next;
  }
  printf("\n");
}
