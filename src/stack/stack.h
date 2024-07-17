#pragma once
#define DEPTH 25
#include "../num_dllist/num_dllist.h"

typedef struct {
  num_dllist_t *res[DEPTH];
  uint8_t top;
} stack_t;

stack_t *stack_init(uint8_t *exit_code);

void stack_push(stack_t *stack, num_dllist_t *element, uint8_t *exit_code);

num_dllist_t *stack_pop(stack_t *stack, uint8_t *exit_code);

void stack_clear(stack_t *stack, uint8_t *exit_code);
