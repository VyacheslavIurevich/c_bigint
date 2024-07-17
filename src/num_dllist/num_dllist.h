#pragma once
#include <inttypes.h>
#include <stddef.h>
#define DLLIST_CAPACITY 300
#define SIGN_NOT_SET -2

typedef struct num_node_s {
  uint64_t digit;
  struct num_node_s *next;
  struct num_node_s *prev;
} num_node_t;

typedef struct {
  num_node_t *first;
  num_node_t *last;
  int8_t sign: 2; // sign is -2 if sign isn't set, -1 if number < 0, 0 if number ==
               // 0, 1 if number > 0
  size_t size;
  size_t capacity;
} num_dllist_t;

static num_dllist_t *dllist_init(uint8_t *exit_code);

static void dllist_clear(num_dllist_t *dllist, uint8_t *exit_code);

static void dllist_add_start(num_dllist_t *dllist, uint64_t element,
                             uint8_t *exit_code);

static void dllist_add_end(num_dllist_t *dllist, uint64_t element,
                           uint8_t *exit_code);

static void dllist_print(num_dllist_t *dllist, uint8_t *exit_code);
