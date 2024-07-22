#include "stack.h"
#include "../exit_codes.h"
#include <stdio.h>
#include <stdlib.h>

stack_t *stack_init(uint8_t *exit_code) {
  stack_t *stack = (stack_t *)malloc(sizeof(stack_t));

  if (stack == NULL) {
    fprintf(stderr, "Cannot initialize stack\n");
    *exit_code = other_errors;
    return NULL;
  }

  stack->top = 0;

  return stack;
}

void stack_push(stack_t *stack, dllist_t *element, uint8_t *exit_code) {
  if ((stack == NULL) || (element == NULL)) {
    fprintf(stderr, "Cannot push element to stack\n");
    *exit_code = other_errors;
    return;
  }

  if (stack->top < DEPTH) {
    stack->res[stack->top] = element;
    ++stack->top;
  } else {
    fprintf(stderr, "Stack is full, cannot push element\n");
    *exit_code = other_errors;
    return;
  }
}

dllist_t *stack_pop(stack_t *stack, uint8_t *exit_code) {
  dllist_t *elem;

  if (stack->top) {
    --stack->top;
    elem = stack->res[stack->top];
    return elem;
  } else {
    fprintf(stderr, "Cannot pop from empty stack\n");
    *exit_code = other_errors;
    return NULL;
  }
}

void stack_clear(stack_t *stack, uint8_t *exit_code) {
  dllist_t *temp;

  if (stack == NULL) {
    fprintf(stderr, "Cannot clear stack\n");
    *exit_code = other_errors;
  }

  while (stack->top) {
    temp = stack_pop(stack, exit_code);
    dllist_clear(temp, exit_code);
  }

  free(stack);
}
