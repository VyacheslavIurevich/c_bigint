#include "calculator/calculator.h"
#include "exit_codes.h"
#include <inttypes.h>

int main(int argc, char **argv) {
  uint8_t exit_code = all_fine;

  parse_commandline_args(argc, argv, &exit_code);
  if (exit_code == all_fine)
    calculate_input(&exit_code);

  return exit_code;
}
