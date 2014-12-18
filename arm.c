/* arm backend for tcasm */

#include "config.h"

#include "tcasm.h"

/* arm addressing modes, as available to instrctions */
enum asm_arm_mode_e
{
  ARM_REG8     = 0x0001, /*a simple register, r0-r7*/
  ARM_REG      = 0x0002, /*a simple register, r0-r15,pc,sp,lr*/
  ARM_PC       = 0x0004, /*pc*/
  ARM_SP       = 0x0008, /*sp*/
  ARM_LIT3     = 0x0010, /*a #value*/
  ARM_LIT5     = 0x0020, /*a #value*/
  ARM_LIT8     = 0x0040, /*a #value*/
  ARM_PCR8     = 0x0080, /*R15 and a displacement [pc, #val8] */
  ARM_SPR8     = 0x0100, /*R13 and a displacement [sp, #val8] */
  ARM_RDS5     = 0x0200, /*a register and a literal displacement [rn, #val] */
  ARM_RRD      = 0x0400, /*a register and a register displacement [rb, ro] */
  ARM_RLIST7LR = 0x0800, /* list of regs in range r0..r7,lr*/
  ARM_RLIST7PC = 0x1000, /* list of regs in range r0..r7,pc*/
  ARM_RLIST7   = 0x2000, /* list of regs in range r0..r7*/
  ARM_LABEL8   = 0x4000,
  ARM_LABEL11  = 0x8000,
};

/* arm instructions */

struct arm_inst
{
  char *name;
  uint8_t argc;
  uint16_t arg1,arg2,arg3;
};


/* ARM7TDMI THUMB instructions */

struct arm_inst arm_thumb_instructions[] =
{
  {"adc"  ,  2, ARM_REG8, ARM_REG8},
  {"add"  ,  3, ARM_REG8, ARM_REG8, ARM_REG8},
  {"add"  ,  3, ARM_REG8, ARM_REG8, ARM_LIT3},
  {"add"  ,  2, ARM_REG8, ARM_LIT8},
  {"add"  ,  2, ARM_REG , ARM_REG },
  {"add"  ,  3, ARM_REG8 , ARM_PC  , ARM_LIT8 },
  {"add"  ,  3, ARM_REG8 , ARM_SP  , ARM_LIT8 },
  {"add"  ,  2, ARM_SP  , ARM_LIT8 }, /*signed value */
  {"and"  ,  2, ARM_REG8, ARM_REG8},
  {"asr"  ,  3, ARM_REG8, ARM_REG8, ARM_LIT5},
  {"asr"  ,  2, ARM_REG8, ARM_REG8},
  {"b"    ,  1, ARM_LABEL11},
  {"beq"  ,  1, ARM_LABEL8},
  {"bne"  ,  1, ARM_LABEL8},
  {"bcs"  ,  1, ARM_LABEL8},
  {"bcc"  ,  1, ARM_LABEL8},
  {"bmi"  ,  1, ARM_LABEL8},
  {"bpl"  ,  1, ARM_LABEL8},
  {"bvs"  ,  1, ARM_LABEL8},
  {"bvc"  ,  1, ARM_LABEL8},
  {"bhi"  ,  1, ARM_LABEL8},
  {"bls"  ,  1, ARM_LABEL8},
  {"bge"  ,  1, ARM_LABEL8},
  {"blt"  ,  1, ARM_LABEL8},
  {"bgt"  ,  1, ARM_LABEL8},
  {"ble"  ,  1, ARM_LABEL8},
  {"bic"  ,  2, ARM_REG8, ARM_REG8},
  {"bl"   ,  1, ARM_LABEL11},
  {"bx"   ,  2, ARM_REG , ARM_REG },
  {"cmn"  ,  2, ARM_REG8, ARM_REG8},
  {"cmp"  ,  2, ARM_REG8, ARM_LIT8},
  {"cmp"  ,  2, ARM_REG8, ARM_REG8},
  {"cmp"  ,  2, ARM_REG , ARM_REG },
  {"eor"  ,  2, ARM_REG8, ARM_REG8},
  {"ldmia",  2, ARM_REG8, ARM_RLIST7},
  {"ldr"  ,  2, ARM_REG8, ARM_PCR8},
  {"ldr"  ,  2, ARM_REG8, ARM_RRD },
  {"ldr"  ,  2, ARM_REG8, ARM_RDS5},
  {"ldr"  ,  2, ARM_REG8, ARM_SPR8},
  {"ldrb" ,  2, ARM_REG8, ARM_RRD },
  {"ldrb" ,  2, ARM_REG8, ARM_RDS5},
  {"ldrh" ,  2, ARM_REG8, ARM_RRD },
  {"ldrh" ,  2, ARM_REG8, ARM_RDS5},
  {"ldsb" ,  2, ARM_REG8, ARM_RRD },
  {"ldsh" ,  2, ARM_REG8, ARM_RRD },
  {"lsl"  ,  3, ARM_REG8, ARM_REG8, ARM_LIT5},
  {"lsl"  ,  2, ARM_REG8, ARM_REG8},
  {"lsr"  ,  3, ARM_REG8, ARM_REG8, ARM_LIT5},
  {"lsr"  ,  2, ARM_REG8, ARM_REG8},
  {"mov"  ,  2, ARM_REG8, ARM_LIT8},
  {"mov"  ,  2, ARM_REG , ARM_REG },
  {"mul"  ,  2, ARM_REG8, ARM_REG8},
  {"mvn"  ,  2, ARM_REG8, ARM_REG8},
  {"neg"  ,  2, ARM_REG8, ARM_REG8},
  {"orr"  ,  2, ARM_REG8, ARM_REG8},
  {"pop"  ,  1, ARM_RLIST7PC},
  {"push" ,  1, ARM_RLIST7LR},
  {"ror"  ,  2, ARM_REG8, ARM_REG8},
  {"sbc"  ,  2, ARM_REG8, ARM_REG8},
  {"stmia",  2, ARM_REG8, ARM_RLIST7},
  {"str"  ,  2, ARM_REG8, ARM_PCR8},
  {"str"  ,  2, ARM_REG8, ARM_RRD },
  {"str"  ,  2, ARM_REG8, ARM_RDS5},
  {"str"  ,  2, ARM_REG8, ARM_SPR8},
  {"strb" ,  2, ARM_REG8, ARM_RRD },
  {"strb" ,  2, ARM_REG8, ARM_RDS5},
  {"strh" ,  2, ARM_REG8, ARM_RRD },
  {"strh" ,  2, ARM_REG8, ARM_RDS5},
  {"stsb" ,  2, ARM_REG8, ARM_RRD },
  {"sub"  ,  2, ARM_REG8, ARM_REG8},
  {"sub"  ,  2, ARM_REG8, ARM_LIT3},
  {"sub"  ,  2, ARM_REG8, ARM_LIT8},
  {"swi"  ,  1, ARM_LIT8},
  {"tst"  ,  2, ARM_REG8, ARM_REG8},
};

struct arm_operand_s
{
  uint16_t type;     /*recognized types*/
  uint8_t  reg  : 4; /*recognized register (main)*/
  uint8_t  regd : 4; /*recognized register (displacement)*/
  uint32_t value;    /*immediate value, label, or reg list */
};

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
  infos->align_p2 = 1; /* align boundaries to power of twos */
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

#define COUNT(tab) (sizeof(tab)/sizeof(tab[0]))

char * arm_parse_operand(struct asm_state_s *state, char *buf, struct arm_operand_s *op)
{
  char *arg;

  /* eat separators */
  while(*buf && (*buf==' ' || *buf=='\t' || *buf==',')) buf++;
  arg = buf;

  if (*arg=='r' || *arg=='p' || *arg=='s' || *arg=='l' || *arg=='#')
    {
      /*reg,pc,sp,lr*/
      while(*buf && !(*buf==' ' || *buf=='\t' || *buf==',')) buf++;
      if(*buf)
        {
          *buf = 0;
          buf++;
        }
      printf("arg: %s\n",arg);
    }
  else if(*arg=='[' || *arg=='{')
    {
      char sep = *arg;

      /*compute closing char */
      sep = (*arg=='[')?']':'}';

      /*[ra,rb], [ra,#imm], {ra,...}*/
      arg++; /* skip initial bracket */
      while(*buf && !(*buf==sep)) buf++;
      if(*buf)
        {
          *buf = 0;
          buf++;
        }
      printf("arg: %s\n",arg);
    }
  else
    {
      emit_message(state, ASM_ERROR, "Syntax error near '%s'", arg);
      return NULL;
    }
  return buf;
}


int arm_instruction(const struct asm_backend_s *backend, struct asm_state_s *state, char *buf)
{
  struct arm_operand_s operands[3];
  int nops;
  int i;
  char *inst = buf;

  printf("arm instruction: %s\n",buf);

  while(*buf && !(*buf==' ' || *buf=='\t')) buf++;
  *buf = 0;
  buf++;

  printf("opcode: %s\n",inst);

  /* parse operands */
  nops = 0;
  while (*buf)
    {
      buf = arm_parse_operand(state, buf, &operands[nops]);
      if(!buf) return ASM_ERROR;
      nops++;
    }

  /* try to match something */

  for (i=0; i<COUNT(arm_thumb_instructions); i++)
    {
    }

  return ASM_OK;
}

