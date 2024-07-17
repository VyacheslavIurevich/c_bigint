#pragma once
#define DEPTH 25
#include "../dllist/dllist.h"

typedef struct {
  dllist_t *res[DEPTH];
  uint8_t top;
} stack_t;

stack_t *stack_init(uint8_t *exit_code);

void stack_push(stack_t *stack, dllist_t *element, uint8_t *exit_code);

dllist_t *stack_pop(stack_t *stack, uint8_t *exit_code);

void stack_clear(stack_t *stack, uint8_t *exit_code);
