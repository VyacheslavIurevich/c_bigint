#pragma once
#include "../dllist/dllist.h"
#include <stdbool.h>
#define BASE                                                                   \
  9 // my numerical system base is 10^9, if you change it nothing will work
#define TEN_POWER_BASE 1000000000

bool is_bigger(dllist_t *first_num, dllist_t *second_num,
                      uint8_t *exit_code);

dllist_t *zero(uint8_t *exit_code);

dllist_t *addition(dllist_t *first_num, dllist_t *second_num,
                          bool negative, uint8_t *exit_code);

dllist_t *subtraction(dllist_t *first_num, dllist_t *second_num,
                             bool negative, uint8_t *exit_code);

dllist_t *multiplication(dllist_t *first_num, dllist_t *second_num,
                                int8_t sign, uint8_t *exit_code);

dllist_t *calculate_operation_result(dllist_t *first_num,
                                            dllist_t *second_num,
                                            char operation, uint8_t *exit_code);

bool isoperation(char ch);

void parse_commandline_args(int argc, char **argv, uint8_t *exit_code);

void calculate_input(uint8_t *exit_code);
