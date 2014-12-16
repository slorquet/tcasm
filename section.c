#include "config.h"

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

  /* search for section. if found, return it */

  for (i = 0; i<CONFIG_ASM_SEC_MAX; i++)
    {
      if (!strcmp(asmstate->sections[i].name, secname) )
        {
          printf("section '%s' found\n", secname);
          return &asmstate->sections[i];
        }
    }

  /* else, create it */
  for (i = 0; i<CONFIG_ASM_SEC_MAX; i++)
    {
      if (!asmstate->sections[i].name[0])
        {
          printf("section '%s' initialized\n", secname);
          strncpy(asmstate->sections[i].name, secname, 16);
          asmstate->sections[i].id   = section_find_id(secname);
          asmstate->sections[i].data = NULL;
          return &asmstate->sections[i];
        }
    }

  /* no section found and no room to initialize */

  emit_message(asmstate, ASM_ERROR, "Too many sections, cannot create '%s'", secname);
  return NULL;
}

