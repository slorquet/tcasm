/* arm backend for tcasm */

#include "config.h"

#include "tcasm.h"

/* forward declarations */
int arm_getinfos(struct asm_backend_infos_s *infos);
int arm_directive(const struct asm_backend_s *backend, struct asm_state_s *state, char *buf);
int arm_instruction(const struct asm_backend_s *backend, struct asm_state_s *state, char *buf);

const struct asm_backend_s arm_backend = 
{
  arm_getinfos,
  arm_directive,
  arm_instruction
};

int arm_getinfos(struct asm_backend_infos_s *infos)
{
  infos->name = "arm";
  infos->endianess = ASM_ENDIAN_LITTLE;
  infos->wordsize = 4; /* 32-bit int and longs */
  return ASM_OK;
}

int arm_directive(const struct asm_backend_s *backend, struct asm_state_s *state, char *dir)
{
  int ret = ASM_UNHANDLED;
  printf("arm directive: %s\n",dir);
  if(!strcmp(dir, ".thumb"))
    {
      ret = ASM_OK;
    }
  return ret;
}

int arm_instruction(const struct asm_backend_s *backend, struct asm_state_s *state, char *buf)
{
  printf("arm instruction: %s\n",buf);
  return ASM_OK;
}

