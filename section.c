#include "config.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "tcasm.h"

static int section_find_id(const char *name)
{
  if (!strcmp(name, ".text"))
    {
      return SECTION_TEXT;
    }
  else if (!strcmp(name, ".rodata"))
    {
      return SECTION_RODATA;
    }
  else if (!strcmp(name, ".data"))
    {
      return SECTION_DATA;
    }
  else if (!strcmp(name, ".bss"))
    {
      return SECTION_BSS;
    }
  else
    {
      return SECTION_CUSTOM;
    }
}

struct asm_section_s *section_find_create(struct asm_state_s *asmstate, const char *secname)
{
  int i;

  /* search for section */
  /* if found, return it */

  for (i = 0; i<CONFIG_ASM_SEC_MAX; i++)
    {
      if (!strcmp(asmstate->sections[i].name, secname) )
        {
          return &asmstate->sections[i];
        }
    }

  /* find id for special names */

  /* else, create it */
  for (i = 0; i<CONFIG_ASM_SEC_MAX; i++)
    {
      if (!asmstate->sections[i].name[0])
        {
          strncpy(asmstate->sections[i].name, secname, 16);
          asmstate->sections[i].id = section_find_id(secname);
          return &asmstate->sections[i];
        }
    }

  /* no slot found */

  emit_message(asmstate, ASM_ERROR, "Too many section, cannot create '%s'", secname);
  return NULL;
}

