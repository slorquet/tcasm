#include "config.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "tcasm.h"

int parse_section(struct asm_state_s *state, const char *secname)
{
  printf("section [%s]\n", secname);
  state->current_section = section_find_create(secname);

  return ASM_OK;
}

