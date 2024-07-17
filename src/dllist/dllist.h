#pragma once
#include <inttypes.h>
#include <stddef.h>
#define DLLIST_CAPACITY 300
#define SIGN_NOT_SET -2

typedef struct dllist_node_s {
  uint64_t digit;
  struct dllist_node_s *next;
  struct dllist_node_s *prev;
} dllist_node_t;

typedef struct {
  dllist_node_t *first;
  dllist_node_t *last;
  int8_t sign : 2; // sign is -2 if sign isn't set, -1 if number < 0, 0 if
                   // number == 0, 1 if number > 0
  size_t size;
  size_t capacity;
} dllist_t;

dllist_t *dllist_init(uint8_t *exit_code);

void dllist_clear(dllist_t *dllist, uint8_t *exit_code);

void dllist_add_start(dllist_t *dllist, uint64_t element, uint8_t *exit_code);

void dllist_add_end(dllist_t *dllist, uint64_t element, uint8_t *exit_code);

void dllist_print(dllist_t *dllist, uint8_t *exit_code);
