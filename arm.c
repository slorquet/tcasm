/* arm backend for tcasm */

#include "config.h"

#include "tcasm.h"

/* arm arch and cpu                                   ISAs
 * armv4t       ARM7TDMI, ARM9TDMI                    ARM, Thumb
 * armv5        ARM7EJ, ARM9E, ARM10E, XScale         x
 * armv6        ARM11                                 x
 * armv6m       cm0, cm0+, cm1                        x
 * armv7m       cm3                                   Thumb2
 * armv7em      cm4, cm7                              Thumb2
 * armv7r       cr4, cr5, cr7                         x
 * armv7a       ca5, ca7, ca8, ca9, ca12, ca15, ca17  x
 * armv8a       ca53, ca75                            x
 */


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

/* https://sourceware.org/binutils/docs/as/ARM-Directives.html */
int arm_directive(const struct asm_backend_s *backend, struct asm_state_s *state, char *dir)
{
  int ret = ASM_UNHANDLED;
  printf("arm directive: %s\n",dir);
  if(!strcmp(dir, ".thumb"))
    {
      ret = ASM_OK;
    }
  else if(!strcmp(dir, ".arm"))
    {
      ret = ASM_OK;
    }
  else if(!strcmp(dir, ".code")) /*[16|32]*/
    {
      ret = ASM_OK;
    }
  /*
   * .even -> .align 2 
   * .pool/.ltorg : http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0041c/Chedgddh.html
   * .syntax unified|divided : https://sourceware.org/binutils/docs/as/ARM_002dInstruction_002dSet.html#ARM_002dInstruction_002dSet
   * .req .unreq -> special treatment, label is not a label but a reg name ! may have to remove the last label 
   */ 
 
  return ret;
}

#define COUNT(tab) (sizeof(tab)/sizeof(tab[0]))

char * arm_parse_operand(struct asm_state_s *state, char *buf, struct arm_operand_s *op)
{
  char *arg;
  long val;

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
      if (*arg=='r')
        {
          val = strtol(arg+1, NULL, 10);
          if (val<0 || val>15)
            {
              goto linvalidreg;
            }
lvalidreg:
          op->type = ARM_REG;
          op->reg = val;
          /* check special regs */
          if(val<8)
            {
              op->type |= ARM_REG8;
            }
          else if(val==13)
            {
              op->type |= ARM_SP;
            }
          else if(val==15)
            {
              op->type |= ARM_PC;
            }
          printf("register %d, flags %04X\n", op->reg, op->type);
        }
      else if (*arg == '#') /*TODO unified syntax does not require litterals to start with a # */
        {
          uint32_t val;
          char *rest;
          /* litteral */
          arg++;
          val = strtol(arg, &rest, 0);
          /*check that no strange characters appear after the litteral*/
          if (*rest)
            {
              emit_message(state, ASM_ERROR, "Syntax error in litteral near '%s'",arg);
              return NULL;
            }
          printf("litteral %s->%u\n",arg,val);
          op->value = val;
          /* set types according to value range */
        }
      else if (arg[2])
        {
          /*after Rn and litterals, only allowed values are pc,sp, or lr*/
linvalidreg:
          emit_message(state, ASM_ERROR, "Invalid register %s", arg);
          return NULL;
        }
      else if (arg[0]=='s' && arg[1]=='p')
        {
          val = 13; /* sp = r13 */
          goto lvalidreg;
        }
      else if (arg[0]=='l' && arg[1]=='r')
        {
          val = 14; /* lr = r14 */
          goto lvalidreg;
        }
      else if (arg[0]=='p' && arg[1]=='c')
        {
          val = 15; /* pc = r15 */
          goto lvalidreg;
        }
      else
        {
          /* catch-all for undefined cases */
          emit_message(state, ASM_ERROR, "Syntax error for operand '%s'", arg);
          return NULL;
        }

    }
  else if(*arg=='[' || *arg=='{')
    {
      char sep = *arg;
      struct arm_operand_s tmp;
      int count;
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

      /* recursively parse the contents of the arg
       * we count the args and only expect 2.
       * Also the first one has to be a reg. */
      count = 0;
      while (*arg)
        {
          arg = arm_parse_operand(state, arg, &tmp);
          if(!arg) return arg;
          count++;
        }
      printf("composite done\n");
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

