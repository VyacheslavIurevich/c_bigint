#include "calculator.h"
#include "../dllist/dllist.h"
#include "../exit_codes.h"
#include "../queue/queue.h"
#include "../stack/stack.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_bigger(dllist_t *first_num, dllist_t *second_num, uint8_t *exit_code) {
  dllist_node_t *temp_first = first_num->first,
                *temp_second = second_num->first;

  if ((first_num == NULL) || (second_num == NULL)) {
    fprintf(stderr, "Cannot compare numbers\n");
    *exit_code = other_errors;
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

dllist_t *zero(uint8_t *exit_code) {
  dllist_t *result = dllist_init(exit_code);
  dllist_add_start(result, 0, exit_code);
  result->sign = 0;
  return result;
}

dllist_t *addition(dllist_t *first_num, dllist_t *second_num, bool negative,
                   uint8_t *exit_code) {
  dllist_t *result = dllist_init(exit_code);
  dllist_node_t *temp_first = first_num->last, *temp_second = second_num->last;
  uint64_t digit_sum;
  uint8_t add_to_digit = 0;
  size_t first_size = first_num->size, second_size = second_num->size,
         size_diff;

  if ((first_num == NULL) || (second_num == NULL)) {
    fprintf(stderr, "Cannot add numbers\n");
    *exit_code = other_errors;
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

dllist_t *subtraction(dllist_t *first_num, dllist_t *second_num, bool negative,
                      uint8_t *exit_code) {
  dllist_t *result = dllist_init(exit_code);
  dllist_node_t *temp_first = first_num->last, *temp_second = second_num->last,
                *search_node;
  uint64_t first_digit = temp_first->digit, second_digit = temp_second->digit,
           search_digit, digit_diff;
  uint8_t changed_digits = 0;
  bool to_reduce = false;
  if ((first_num == NULL) || (second_num == NULL)) {
    fprintf(stderr, "Cannot subtract numbers\n");
    *exit_code = other_errors;
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

dllist_t *multiplication(dllist_t *first_num, dllist_t *second_num, int8_t sign,
                         uint8_t *exit_code) {
  dllist_t *cur_num, *result = dllist_init(exit_code), *temp;
  dllist_node_t *temp_first = first_num->last, *temp_second;
  uint64_t first_digit, second_digit, partial = 0, remainder = 0,
                                      digits_product;
  uint16_t indent = 0, indent_idx;
  if ((first_num == NULL) || (second_num == NULL)) {
    fprintf(stderr, "Cannot multiplicate numbers\n");
    *exit_code = other_errors;
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

dllist_t *calculate_operation_result(dllist_t *first_num, dllist_t *second_num,
                                     char operation, uint8_t *exit_code) {
  if ((first_num == NULL) || (second_num == NULL)) {
    fprintf(stderr, "Cannot calculate operation result\n");
    *exit_code = other_errors;
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

bool isoperation(char ch) { return ((ch == '+' || ch == '-' || ch == '*')); }

void parse_commandline_args(int argc, char **argv, uint8_t *exit_code) {
  if (argc == 2) {
    if (strcmp(argv[1], "--revpol") != 0) {
      if (strcmp(argv[1], "--infix") == 0) {
        fprintf(stderr, "This app doesn't support infix notation\n");
        *exit_code = no_infix;
      } else {
        fprintf(stderr, "Incorrect command line argument\n");
        *exit_code = commandline_args_error;
      }
    }
  } else {
    if (argc > 2)
      fprintf(stderr, "Too much command line arguments entered\n");
    else
      fprintf(stderr, "No command line arguments entered\n");
    *exit_code = commandline_args_error;
  }
}

void calculate_input(uint8_t *exit_code) {
  char cur_ch, prev_ch;
  bool convert_flag;
  uint64_t cur_num;
  uint8_t digits_count, add_digit = 0;
  queue_t *temp = NULL;
  queue_node_t *cur_node;
  dllist_t *cur_num_ns = NULL, *result = NULL, *first = NULL, *second = NULL;
  stack_t *results_stack = stack_init(exit_code);

  do {
    cur_ch = getchar();
  } while (cur_ch == ' ');

  if (cur_ch == '\n' || cur_ch == EOF) {
    fprintf(stderr, "Empty input\n");
    *exit_code = input_error;
  }

  prev_ch = cur_ch;
  if (!isdigit(cur_ch) && (cur_ch != '-') && (cur_ch != '\n') &&
      (cur_ch != EOF)) {
    fprintf(stderr,
            "Input must start with a digit, a minus or with a blank space\n");
    *exit_code = input_error;
  } else {
    while (*exit_code == all_fine && (cur_ch = getchar()) != '\n' &&
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
        *exit_code = no_division;
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
            *exit_code = input_error;
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
          *exit_code = input_error;
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
          *exit_code = input_error;
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
            *exit_code = input_error;
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
          *exit_code = input_error;
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
            *exit_code = input_error;
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
          *exit_code = input_error;
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
          *exit_code = input_error;
        }
      }
      prev_ch = cur_ch;
    }
  }
  if ((isdigit(prev_ch)) && (*exit_code == all_fine)) {
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
      *exit_code = input_error;
    }
  }
  if (results_stack) {
    if (results_stack->top == 1) {
      result = stack_pop(results_stack, exit_code);
      if (*exit_code == all_fine)
        dllist_print(result, exit_code);
      if (result)
        dllist_clear(result, exit_code);
      stack_clear(results_stack, exit_code);
    } else if (results_stack->top > 1) {
      fprintf(stderr, "Not enough operations\n");
      *exit_code = input_error;
      stack_clear(results_stack, exit_code);
    } else
      stack_clear(results_stack, exit_code);
  }
  if (temp)
    queue_clear(temp, exit_code);
}
