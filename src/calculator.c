#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define EXITCODE_ALL_FINE 0
#define EXITCODE_COMMANDLINE_ARGS_ERROR 1
#define EXITCODE_NO_INFIX 2
#define EXITCODE_NO_DIVISION 3
#define EXITCODE_INPUT_ERROR 4
#define EXITCODE_OTHER_ERRORS 5
#define QUEUE_CAPACITY 5000
#define DLLIST_CAPACITY 300
#define SIGN_NOT_SET -2
#define BASE                                                                   \
  9 // my numerical system base is 10^9, if you change it nothing will work
#define TEN_POWER_BASE 1000000000
#define DEPTH 25

typedef struct num_node_s {
  uint64_t digit;
  struct num_node_s *next;
  struct num_node_s *prev;
} num_node_t;

typedef struct {
  num_node_t *first;
  num_node_t *last;
  int8_t sign; // sign is -2 if sign isn't set, -1 if number < 0, 0 if number ==
               // 0, 1 if number > 0
  size_t size;
  size_t capacity;
} num_dllist_t;

typedef struct {
  num_dllist_t *res[DEPTH];
  uint8_t top;
} results_stack_t;

typedef struct digits_node_s {
  uint8_t decimal_digit;
  struct digits_node_s *next;
} digits_node_t;

typedef struct {
  digits_node_t *head;
  digits_node_t *tail;
  size_t size;
  size_t capacity;
} digits_queue_t;

static num_dllist_t *dllist_init(uint8_t *exit_code) {
  num_dllist_t *dllist = (num_dllist_t *)malloc(sizeof(num_dllist_t));

  if (dllist == NULL) {
    fprintf(stderr, "Cannot initialize double linked list\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
    return NULL;
  }

  dllist->first = NULL;
  dllist->last = NULL;
  dllist->sign = SIGN_NOT_SET;
  dllist->size = 0;
  dllist->capacity = DLLIST_CAPACITY;

  return dllist;
}

static void dllist_clear(num_dllist_t *dllist, uint8_t *exit_code) {
  num_node_t *temp;

  if (dllist == NULL) {
    fprintf(stderr, "Cannot clear double linked list\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
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

static void dllist_add_start(num_dllist_t *dllist, uint64_t element,
                             uint8_t *exit_code) {
  num_node_t *node = (num_node_t *)malloc(sizeof(num_node_t));

  if ((node == NULL) || (dllist == NULL)) {
    fprintf(stderr, "Cannot add element in the start of double linked list\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
  }

  if (dllist->size == dllist->capacity) {
    fprintf(stderr, "Cannot add element in the start of double linked list "
                    "because it is full\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
  }

  node->digit = element;
  node->next = dllist->first;
  node->prev = NULL;

  if (dllist->first) {
    dllist->first->prev = node;
  }
  dllist->first = node;

  if (dllist->last == NULL) {
    dllist->last = node;
  }

  ++dllist->size;
}

static void dllist_add_end(num_dllist_t *dllist, uint64_t element,
                           uint8_t *exit_code) {
  num_node_t *node = (num_node_t *)malloc(sizeof(num_node_t));

  if ((node == NULL) || (dllist == NULL)) {
    fprintf(stderr,
            "Cannot add element to the end of the double linked list\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
  }

  if (dllist->size == dllist->capacity) {
    fprintf(stderr, "Cannot add element to the end of the double linked list, "
                    "because it is full\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
  }

  node->digit = element;
  node->next = NULL;
  node->prev = dllist->last;

  if (dllist->last) {
    dllist->last->next = node;
  }
  dllist->last = node;

  if (dllist->first == NULL) {
    dllist->first = node;
  }

  ++dllist->size;
}

static void dllist_print(num_dllist_t *dllist, uint8_t *exit_code) {
  num_node_t *temp = dllist->first;
  bool flag = false;

  if (dllist == NULL) {
    fprintf(stderr, "Cannot print answer \n");
    *exit_code = EXITCODE_OTHER_ERRORS;
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

static results_stack_t *stack_init(uint8_t *exit_code) {
  results_stack_t *stack = (results_stack_t *)malloc(sizeof(results_stack_t));

  if (stack == NULL) {
    fprintf(stderr, "Cannot initialize stack\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
    return NULL;
  }

  stack->top = 0;

  return stack;
}

static void stack_push(results_stack_t *stack, num_dllist_t *element,
                       uint8_t *exit_code) {
  if ((stack == NULL) || (element == NULL)) {
    fprintf(stderr, "Cannot push element to stack\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
  }
  if (stack->top < DEPTH) {
    stack->res[stack->top] = element;
    ++stack->top;
  } else {
    fprintf(stderr, "Stack is full, cannot push element\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
  }
}

static num_dllist_t *stack_pop(results_stack_t *stack, uint8_t *exit_code) {
  num_dllist_t *elem;
  if (stack->top) {
    --stack->top;
    elem = stack->res[stack->top];
    return elem;
  } else {
    fprintf(stderr, "Cannot pop from empty stack\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
    return NULL;
  }
}

static void stack_clear(results_stack_t *stack, uint8_t *exit_code) {
  num_dllist_t *temp;

  if (stack == NULL) {
    fprintf(stderr, "Cannot clear stack\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
  }

  while (stack->top) {
    temp = stack_pop(stack, exit_code);
    dllist_clear(temp, exit_code);
  }

  free(stack);
}

static digits_queue_t *queue_init(uint8_t *exit_code) {
  digits_queue_t *queue = (digits_queue_t *)malloc(sizeof(digits_queue_t));

  if (queue == NULL) {
    fprintf(stderr, "Cannot initialize queue\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
    return NULL;
  }

  queue->head = NULL;
  queue->tail = NULL;
  queue->size = 0;
  queue->capacity = QUEUE_CAPACITY;

  return queue;
}

static void queue_clear(digits_queue_t *queue, uint8_t *exit_code) {
  digits_node_t *temp;

  if (queue == NULL) {
    fprintf(stderr, "Cannot clear queue\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
  }

  while (queue->head != NULL) {
    temp = queue->head;
    queue->head = queue->head->next;
    free(temp);
  }

  queue->tail = NULL;
  queue->size = 0;
  queue->capacity = 0;

  free(queue);
}

static void queue_add(digits_queue_t *queue, uint8_t element,
                      uint8_t *exit_code) {
  digits_node_t *node = (digits_node_t *)malloc(sizeof(digits_node_t));

  if (node == NULL) {
    fprintf(stderr, "Cannot create queue node\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
  }

  if (queue->size == queue->capacity) {
    fprintf(stderr, "Queue is full, cannot add new node\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
  }

  node->decimal_digit = element;
  node->next = NULL;

  if (queue->head == NULL) {
    // we had an empty queue and now the queue consists of 1 node
    queue->head = node;
    queue->tail = node;
    ++queue->size;
  } else {
    queue->tail->next = node;
    queue->tail = node;
    ++queue->size;
  }
}

static bool is_bigger(num_dllist_t *first_num, num_dllist_t *second_num,
                      uint8_t *exit_code) {
  num_node_t *temp_first = first_num->first, *temp_second = second_num->first;

  if ((first_num == NULL) || (second_num == NULL)) {
    fprintf(stderr, "Cannot compare numbers\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
  }

  if (first_num->size > second_num->size)
    return true;
  else if (first_num->size < second_num->size)
    return false;
  else {
    while ((temp_first) && (temp_second)) {
      if (temp_first->digit > temp_second->digit)
        return true;
      else if (temp_first->digit < temp_second->digit)
        return false;
      else {
        temp_first = temp_first->next;
        temp_second = temp_second->next;
      }
    }
    return false;
  }
}

static num_dllist_t *zero(uint8_t *exit_code) {
  num_dllist_t *result = dllist_init(exit_code);
  dllist_add_start(result, 0, exit_code);
  result->sign = 0;
  return result;
}

static num_dllist_t *addition(num_dllist_t *first_num, num_dllist_t *second_num,
                              bool negative, uint8_t *exit_code) {
  num_dllist_t *result = dllist_init(exit_code);
  num_node_t *temp_first = first_num->last, *temp_second = second_num->last;
  uint64_t digit_sum;
  uint8_t add_to_digit = 0;
  size_t first_size = first_num->size, second_size = second_num->size,
         size_diff;

  if ((first_num == NULL) || (second_num == NULL)) {
    fprintf(stderr, "Cannot add numbers\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
    return NULL;
  }

  if (first_size < second_size) {
    for (size_diff = second_size - first_size; size_diff; --size_diff) {
      dllist_add_start(first_num, 0, exit_code);
    }
  } else if (first_size > second_size) {
    for (size_diff = first_size - second_size; size_diff; --size_diff) {
      dllist_add_start(second_num, 0, exit_code);
    }
  }

  while ((temp_first) && (temp_second)) {
    digit_sum = temp_first->digit + temp_second->digit + add_to_digit;
    if (digit_sum >= TEN_POWER_BASE) {
      add_to_digit = 1;
      digit_sum -= TEN_POWER_BASE;
    } else
      add_to_digit = 0;
    dllist_add_start(result, digit_sum, exit_code);
    temp_first = temp_first->prev;
    temp_second = temp_second->prev;
  }
  if (add_to_digit)
    dllist_add_start(result, 1, exit_code);

  result->sign = ((negative) ? -1 : 1);

  return result;
}

static num_dllist_t *subtraction(num_dllist_t *first_num,
                                 num_dllist_t *second_num, bool negative,
                                 uint8_t *exit_code) {
  num_dllist_t *result = dllist_init(exit_code);
  num_node_t *temp_first = first_num->last, *temp_second = second_num->last,
             *search_node;
  uint64_t first_digit = temp_first->digit, second_digit = temp_second->digit,
           search_digit, digit_diff;
  uint8_t changed_digits = 0;
  bool to_reduce = false;
  if ((first_num == NULL) || (second_num == NULL)) {
    fprintf(stderr, "Cannot subtract numbers\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
    return NULL;
  }

  if (first_num->size > second_num->size) {
    for (size_t size_diff = first_num->size - second_num->size; size_diff;
         --size_diff) {
      dllist_add_start(second_num, 0, exit_code);
    }
  }

  while ((temp_first) && (temp_second)) {
    first_digit = temp_first->digit;
    second_digit = temp_second->digit;
    if ((first_digit == 0) && (changed_digits > 0)) {
      first_digit = TEN_POWER_BASE - 1;
      --changed_digits;
    } else if ((first_digit > 0) && (to_reduce)) {
      --first_digit;
      to_reduce = false;
    }
    if (first_digit >= second_digit)
      digit_diff = first_digit - second_digit;
    else {
      search_node = temp_first->prev;
      search_digit = search_node->digit;
      while (search_digit == 0) {
        ++changed_digits;
        search_node = temp_first->prev;
        search_digit = search_node->digit;
      }
      to_reduce = true;
      digit_diff = first_digit + TEN_POWER_BASE - second_digit;
    }
    dllist_add_start(result, digit_diff, exit_code);
    temp_first = temp_first->prev;
    temp_second = temp_second->prev;
  }

  result->sign = ((negative) ? -1 : 1);
  if ((result->size == 1) && (result->first->digit == 0))
    result->sign = 0;
  return result;
}

static num_dllist_t *multiplication(num_dllist_t *first_num,
                                    num_dllist_t *second_num, int8_t sign,
                                    uint8_t *exit_code) {
  num_dllist_t *cur_num, *result = dllist_init(exit_code), *temp;
  num_node_t *temp_first = first_num->last, *temp_second;
  uint64_t first_digit, second_digit, partial = 0, remainder = 0,
                                      digits_product;
  uint16_t indent = 0, indent_idx;
  if ((first_num == NULL) || (second_num == NULL)) {
    fprintf(stderr, "Cannot multiplicate numbers\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
    return NULL;
  }

  dllist_add_end(result, 0, exit_code);

  while (temp_first) {
    first_digit = temp_first->digit;
    temp_second = second_num->last;
    cur_num = dllist_init(exit_code);
    cur_num->sign = 1;
    if (indent) {
      for (indent_idx = 0; indent_idx < indent; ++indent_idx) {
        dllist_add_start(cur_num, 0, exit_code);
      }
    }
    while (temp_second) {
      second_digit = temp_second->digit;
      digits_product = first_digit * second_digit + partial;
      remainder = digits_product % TEN_POWER_BASE;
      partial = digits_product / TEN_POWER_BASE;
      dllist_add_start(cur_num, remainder, exit_code);
      temp_second = temp_second->prev;
    }
    if (partial) {
      dllist_add_start(cur_num, partial, exit_code);
      partial = 0;
    }
    temp = addition(result, cur_num, false, exit_code);
    dllist_clear(result, exit_code);
    result = temp;
    ++indent;
    dllist_clear(cur_num, exit_code);
    temp_first = temp_first->prev;
  }

  result->sign = sign;

  return result;
}

static num_dllist_t *calculate_operation_result(num_dllist_t *first_num,
                                                num_dllist_t *second_num,
                                                char operation,
                                                uint8_t *exit_code) {
  if ((first_num == NULL) || (second_num == NULL)) {
    fprintf(stderr, "Cannot calculate operation result\n");
    *exit_code = EXITCODE_OTHER_ERRORS;
    return NULL;
  } else if (operation == '+') {
    if (first_num->sign == 1) {
      if (second_num->sign != -1)
        return addition(first_num, second_num, false, exit_code);
      else {
        if (is_bigger(first_num, second_num, exit_code))
          return subtraction(first_num, second_num, false, exit_code);
        else
          return subtraction(second_num, first_num, true, exit_code);
      }
    } else if (first_num->sign == 0) {
      if (second_num->sign != 0)
        return addition(first_num, second_num,
                        ((second_num->sign == 1) ? false : true), exit_code);
      else
        return zero(exit_code);
    } else {
      if (second_num->sign == 1) {
        if (is_bigger(first_num, second_num, exit_code))
          return subtraction(first_num, second_num, true, exit_code);
        else
          return subtraction(second_num, first_num, false, exit_code);
      } else if (second_num->sign == 0)
        return addition(first_num, second_num, true, exit_code);
      else
        return addition(first_num, second_num, true, exit_code);
    }
  } else if (operation == '-') {
    if (first_num->sign == 1) {
      if (second_num->sign == 1) {
        if (is_bigger(first_num, second_num, exit_code))
          return subtraction(first_num, second_num, false, exit_code);
        else
          return subtraction(second_num, first_num, true, exit_code);
      } else
        return addition(first_num, second_num, false, exit_code);
    } else if (first_num->sign == 0) {
      if (second_num->sign != 0)
        return addition(second_num, first_num,
                        ((second_num->sign == 1) ? true : false), exit_code);
      else
        return zero(exit_code);
    } else if (first_num->sign == -1) {
      if (second_num->sign == 1)
        return addition(first_num, second_num, true, exit_code);
      else if (second_num->sign == 0)
        return addition(first_num, second_num, true, exit_code);
      else {
        if (is_bigger(first_num, second_num, exit_code))
          return subtraction(first_num, second_num, true, exit_code);
        else
          return subtraction(second_num, first_num, false, exit_code);
      }
    }
  } else if (operation == '*') {
    if (first_num->sign == 1) {
      if (second_num->sign == 1)
        return multiplication(first_num, second_num, 1, exit_code);
      else if (second_num->sign == 0)
        return zero(exit_code);
      else
        return multiplication(first_num, second_num, -1, exit_code);
    } else if (first_num->sign == 0)
      return zero(exit_code);
    else {
      if (second_num->sign == 1)
        return multiplication(first_num, second_num, -1, exit_code);
      else if (second_num->sign == 0)
        return zero(exit_code);
      else
        return multiplication(first_num, second_num, 1, exit_code);
    }
  }
  return NULL;
}

static bool isoperation(char ch) {
  return ((ch == '+' || ch == '-' || ch == '*'));
}

static void parse_commandline_args(int argc, char **argv, uint8_t *exit_code) {
  if (argc == 2) {
    if (strcmp(argv[1], "--revpol") != 0) {
      if (strcmp(argv[1], "--infix") == 0) {
        fprintf(stderr, "This app doesn't support infix notation\n");
        *exit_code = EXITCODE_NO_INFIX;
      } else {
        fprintf(stderr, "Incorrect command line argument\n");
        *exit_code = EXITCODE_COMMANDLINE_ARGS_ERROR;
      }
    }
  } else {
    if (argc > 2)
      fprintf(stderr, "Too much command line arguments entered\n");
    else
      fprintf(stderr, "No command line arguments entered\n");
    *exit_code = EXITCODE_COMMANDLINE_ARGS_ERROR;
  }
}

static void parse_input(uint8_t *exit_code) {
  char cur_ch, prev_ch;
  bool convert_flag;
  uint64_t cur_num;
  uint8_t digits_count, add_digit = 0;
  digits_queue_t *temp = NULL;
  digits_node_t *cur_node;
  num_dllist_t *cur_num_ns = NULL, *result = NULL, *first = NULL,
               *second = NULL;
  results_stack_t *results_stack = stack_init(exit_code);

  do {
    cur_ch = getchar();
  } while (cur_ch == ' ');

  if (cur_ch == '\n' || cur_ch == EOF) {
    fprintf(stderr, "Empty input\n");
    *exit_code = EXITCODE_INPUT_ERROR;
  }

  prev_ch = cur_ch;
  if (!isdigit(cur_ch) && (cur_ch != '-') && (cur_ch != '\n') &&
      (cur_ch != EOF)) {
    fprintf(stderr,
            "Input must start with a digit, a minus or with a blank space\n");
    *exit_code = EXITCODE_INPUT_ERROR;
  } else {
    while (*exit_code == EXITCODE_ALL_FINE && (cur_ch = getchar()) != '\n' &&
           cur_ch != EOF) {
      if (cur_ch == '/') {
        if ((cur_num_ns) && (!results_stack)) {
          dllist_clear(cur_num_ns, exit_code);
          cur_num_ns = NULL;
        }
        if (temp) {
          queue_clear(temp, exit_code);
          temp = NULL;
        }
        if (result) {
          dllist_clear(result, exit_code);
          result = NULL;
        }
        if (results_stack) {
          stack_clear(results_stack, exit_code);
          results_stack = NULL;
        }
        fprintf(stderr, "Division is not supported\n");
        *exit_code = EXITCODE_NO_DIVISION;
      }

      if (isdigit(prev_ch)) {
        if (isdigit(cur_ch)) {
          if ((prev_ch == '0') && ((!temp) || (temp->size == 1))) {
            if ((cur_num_ns) && (!results_stack)) {
              dllist_clear(cur_num_ns, exit_code);
              cur_num_ns = NULL;
            }
            if (temp) {
              queue_clear(temp, exit_code);
              temp = NULL;
            }
            if (result) {
              dllist_clear(result, exit_code);
              result = NULL;
            }
            if (results_stack) {
              stack_clear(results_stack, exit_code);
              results_stack = NULL;
            }
            fprintf(stderr, "A number can't start with a 0\n");
            *exit_code = EXITCODE_INPUT_ERROR;
          } else {
            if (!temp) {
              temp = queue_init(exit_code);
              cur_num_ns = dllist_init(exit_code);
              cur_num_ns->sign = 1;
              add_digit = (uint8_t)(prev_ch - '0');
              queue_add(temp, add_digit, exit_code);
            }
            add_digit = (uint8_t)(cur_ch - '0');
            queue_add(temp, add_digit, exit_code);
          }
        } else if (isoperation(cur_ch)) {
          if (cur_num_ns) {
            dllist_clear(cur_num_ns, exit_code);
            cur_num_ns = NULL;
          }
          if (temp) {
            queue_clear(temp, exit_code);
            temp = NULL;
          }
          if (result) {
            dllist_clear(result, exit_code);
            result = NULL;
          }
          if (results_stack) {
            stack_clear(results_stack, exit_code);
            results_stack = NULL;
          }
          fprintf(stderr, "Missing space between number and operation\n");
          *exit_code = EXITCODE_INPUT_ERROR;
        } else if (cur_ch == ' ') {
          if (!temp) {
            temp = queue_init(exit_code);
            cur_num_ns = dllist_init(exit_code);
            cur_num_ns->sign = ((prev_ch == '0') ? 0 : 1);
            add_digit = (uint8_t)(prev_ch - '0');
            queue_add(temp, add_digit, exit_code);
          }
          // convert into 10^9 NS
          if (temp->size == 1)
            dllist_add_end(cur_num_ns, temp->head->decimal_digit, exit_code);
          else {
            cur_node = temp->head;
            cur_num = 0;
            convert_flag = true;
            digits_count = (9 - temp->size % 9) % 9;
            while (convert_flag) {
              if (cur_node->next == NULL)
                convert_flag = false;
              if (digits_count < BASE) {
                cur_num *= 10;
                cur_num += cur_node->decimal_digit;
                ++digits_count;
              }
              if (digits_count == BASE) {
                dllist_add_end(cur_num_ns, cur_num, exit_code);
                digits_count = 0;
                cur_num = 0;
              }
              if (cur_node->next)
                cur_node = cur_node->next;
            }
          }
          stack_push(results_stack, cur_num_ns, exit_code);
          queue_clear(temp, exit_code);
          temp = NULL;
        } else if (cur_ch != '/') {
          if ((cur_num_ns) && (!results_stack->top)) {
            dllist_clear(cur_num_ns, exit_code);
            cur_num_ns = NULL;
          }
          if (temp) {
            queue_clear(temp, exit_code);
            temp = NULL;
          }
          if (result) {
            dllist_clear(result, exit_code);
            result = NULL;
          }
          if (results_stack) {
            stack_clear(results_stack, exit_code);
            results_stack = NULL;
          }
          fprintf(stderr, "Incorrect characters in input\n");
          *exit_code = EXITCODE_INPUT_ERROR;
        }
      } else if (isoperation(prev_ch)) {
        if (isdigit(cur_ch)) {
          if (prev_ch != '-') {
            if ((cur_num_ns) && (!results_stack)) {
              dllist_clear(cur_num_ns, exit_code);
              cur_num_ns = NULL;
            }
            if (temp) {
              queue_clear(temp, exit_code);
              temp = NULL;
            }
            if (result) {
              dllist_clear(result, exit_code);
              result = NULL;
            }
            if (results_stack) {
              stack_clear(results_stack, exit_code);
              results_stack = NULL;
            }
            fprintf(stderr, "Missing space between number and operation\n");
            *exit_code = EXITCODE_INPUT_ERROR;
          } else {
            temp = queue_init(exit_code);
            cur_num_ns = dllist_init(exit_code);
            add_digit = (uint8_t)(cur_ch - '0');
            queue_add(temp, add_digit, exit_code);
            cur_num_ns->sign = ((cur_ch != '0') ? -1 : 0);
          }
        } else if (isoperation(cur_ch)) {
          if ((cur_num_ns) && (!results_stack)) {
            dllist_clear(cur_num_ns, exit_code);
            cur_num_ns = NULL;
          }
          if (temp) {
            queue_clear(temp, exit_code);
            temp = NULL;
          }
          if (result) {
            dllist_clear(result, exit_code);
            result = NULL;
          }
          if (results_stack) {
            stack_clear(results_stack, exit_code);
            results_stack = NULL;
          }
          fprintf(stderr, "Missing space between operations\n");
          *exit_code = EXITCODE_INPUT_ERROR;
        } else if (cur_ch == ' ') {
          if (results_stack->top > 1) {
            first = stack_pop(results_stack, exit_code);
            second = stack_pop(results_stack, exit_code);
            result =
                calculate_operation_result(second, first, prev_ch, exit_code);
            dllist_clear(first, exit_code);
            dllist_clear(second, exit_code);
            stack_push(results_stack, result, exit_code);
          } else {
            if (temp) {
              queue_clear(temp, exit_code);
              temp = NULL;
            }
            if (result) {
              dllist_clear(result, exit_code);
              result = NULL;
            }
            fprintf(stderr, "Missing numbers in input\n");
            *exit_code = EXITCODE_INPUT_ERROR;
          }
        } else if ((cur_ch != '/') && (cur_ch != ' ')) {
          if ((cur_num_ns) && (!results_stack)) {
            dllist_clear(cur_num_ns, exit_code);
            cur_num_ns = NULL;
          }
          if (temp) {
            queue_clear(temp, exit_code);
            temp = NULL;
          }
          if (result) {
            dllist_clear(result, exit_code);
            result = NULL;
          }
          if (results_stack) {
            stack_clear(results_stack, exit_code);
            results_stack = NULL;
          }
          fprintf(stderr, "Incorrect characters in input\n");
          *exit_code = EXITCODE_INPUT_ERROR;
        }
      } else if (prev_ch == ' ') {
        if (isdigit(cur_ch)) {
          temp = queue_init(exit_code);
          cur_num_ns = dllist_init(exit_code);
          add_digit = (uint8_t)(cur_ch - '0');
          queue_add(temp, add_digit, exit_code);
          cur_num_ns->sign = ((cur_ch == '0') ? 0 : 1);
        } else if ((cur_ch != ' ') && (cur_ch != '/') &&
                   (!isoperation(cur_ch))) {
          if ((cur_num_ns) && (!results_stack)) {
            dllist_clear(cur_num_ns, exit_code);
            cur_num_ns = NULL;
          }
          if (temp) {
            queue_clear(temp, exit_code);
            temp = NULL;
          }
          if (result) {
            dllist_clear(result, exit_code);
            result = NULL;
          }
          if (results_stack) {
            stack_clear(results_stack, exit_code);
            results_stack = NULL;
          }
          fprintf(stderr, "Incorrect characters in input\n");
          *exit_code = EXITCODE_INPUT_ERROR;
        }
      }
      prev_ch = cur_ch;
    }
  }
  if ((isdigit(prev_ch)) && (*exit_code == EXITCODE_ALL_FINE)) {
    if (!temp) {
      temp = queue_init(exit_code);
      cur_num_ns = dllist_init(exit_code);
      cur_num_ns->sign = ((prev_ch == '0') ? 0 : 1);
      add_digit = (uint8_t)(prev_ch - '0');
      queue_add(temp, add_digit, exit_code);
    }
    if (temp->size == 1)
      dllist_add_end(cur_num_ns, temp->head->decimal_digit, exit_code);
    else {
      cur_node = temp->head;
      cur_num = 0;
      convert_flag = true;
      digits_count = (9 - temp->size % 9) % 9;
      while (convert_flag) {
        if (cur_node->next == NULL)
          convert_flag = false;
        if (digits_count < BASE) {
          cur_num *= 10;
          cur_num += cur_node->decimal_digit;
          ++digits_count;
        }
        if (digits_count == BASE) {
          dllist_add_end(cur_num_ns, cur_num, exit_code);
          digits_count = 0;
          cur_num = 0;
        }
        if (cur_node->next)
          cur_node = cur_node->next;
      }
    }
    stack_push(results_stack, cur_num_ns, exit_code);
  }
  if ((isoperation(prev_ch)) && (results_stack)) {
    if (results_stack->top > 1) {
      first = stack_pop(results_stack, exit_code);
      second = stack_pop(results_stack, exit_code);
      result = calculate_operation_result(second, first, prev_ch, exit_code);
      dllist_clear(first, exit_code);
      dllist_clear(second, exit_code);
      stack_push(results_stack, result, exit_code);
    } else {
      if (cur_num_ns) {
        dllist_clear(cur_num_ns, exit_code);
        cur_num_ns = NULL;
      }
      if (temp) {
        queue_clear(temp, exit_code);
        temp = NULL;
      }
      if (result) {
        dllist_clear(result, exit_code);
        result = NULL;
      }
      fprintf(stderr, "Missing numbers in input\n");
      *exit_code = EXITCODE_INPUT_ERROR;
    }
  }
  if (results_stack) {
    if (results_stack->top == 1) {
      result = stack_pop(results_stack, exit_code);
      if (*exit_code == EXITCODE_ALL_FINE)
        dllist_print(result, exit_code);
      if (result)
        dllist_clear(result, exit_code);
      stack_clear(results_stack, exit_code);
    } else if (results_stack->top > 1) {
      fprintf(stderr, "Not enough operations\n");
      *exit_code = EXITCODE_INPUT_ERROR;
      stack_clear(results_stack, exit_code);
    } else
      stack_clear(results_stack, exit_code);
  }
  if (temp)
    queue_clear(temp, exit_code);
}

int main(int argc, char **argv) {
  uint8_t exit_code = EXITCODE_ALL_FINE;

  parse_commandline_args(argc, argv, &exit_code);
  if (exit_code == EXITCODE_ALL_FINE)
    parse_input(&exit_code);

  return exit_code;
}
