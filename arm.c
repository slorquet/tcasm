/* arm backend for tcasm */

#include "config.h"

#include "tcasm.h"

/* forward declarations */
int arm_getinfos(struct asm_backend_infos_s *infos);
int arm_directive(const struct asm_backend_s *backend, struct asm_state_s *state, char *buf);

const struct asm_backend_s arm_backend = 
{
  arm_getinfos,
  arm_directive
};

int arm_getinfos(struct asm_backend_infos_s *infos)
{
  infos->name = "arm";
  infos->endianess = ASM_ENDIAN_LITTLE;
  return ASM_OK;
}

int arm_directive(const struct asm_backend_s *backend, struct asm_state_s *state, char *buf)
{
  return ASM_OK;
}

int arm_instruction(const struct asm_backend_s *backend, struct asm_state_s *state, char *buf)
{
  return ASM_OK;
}

