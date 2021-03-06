/* arm backend for tcasm */

#include "config.h"

#include "tcasm.h"

/* arm arch and cpu                                      ISAs
 * http://www.heyrick.co.uk/armwiki/The_ARM_family
 * http://en.wikipedia.org/wiki/List_of_ARM_microarchitectures
 * armv4        StrongARM, ARM8                          ARM
 * armv4t       ARM7TDMI, ARM9TDMI                       ARM  Thumb
 * armv5t       Xscale, ARM10E, ARM9E, ARM7EJ            ARM  Thumb
 * armv6        ARM11                                    ARM  Thumb
 * armv6t2      ARM11                                    ARM  Thumb
 * armv6m       cm0, cm0+, cm1                                Thumb2    DDI0419C A.4.1 page 66
 * armv7m       cm3                                           Thumb2
 * armv7em      cm4, cm7                                      Thumb2
 * armv7r       cr4, cr5, cr7                            ARM
 * armv7a       ca5, ca7, ca8, ca9, ca12, ca15, ca17     ARM
 * armv8a       ca53, ca75                               ARM
 */

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************/
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

/* instruction formats */
enum arm_format_e
{               /* Bits             Format Desc */
  /* Thumb Instructions formats */

  FMT_TR3,      /* ooooooommmnnnddd     1    opcode Rld, Rln, Rlm | Rld, [Rln, Rlm]*/
  FMT_TI3R2,    /* oooooooiiinnnddd     2    opcode Rld, [Rln, #imm3] */
  FMT_TR1I8,    /* ooooodddiiiiiiii     3    opcode Rld, #imm8 */
  FMT_TI5R2,    /* oooooiiiiinnnddd     4    opcode Rld, [RLn, #imm5] */
  FMT_TR2,      /* oooooooooommmddd     5    opcode Rlm, Rld */
  FMT_TR1PCI8,  /* ooooodddiiiiiiii     6    opcode Rld, [PC, #imm8*4] */
  FMT_TR1SPI8,  /* ooooodddiiiiiiii     6    opcode Rld, [SP, #imm8*4] */
  FMT_TSPI7,    /* oooooooooiiiiiii     7    opcode SP, #imm7 */
  FMT_TRH2,     /* ooooooooDMmmmddd     8    opcode Rhd, Rhm */
  FMT_TC4I8,    /* oooocccciiiiiiii     CB   opcode cond, imm8 */
  FMT_TI11,     /* oooooiiiiiiiiiii     UB   opcode imm11 */
  FMT_TI8,      /* oooooiiiiiiiiiii   MISC   opcode imm8 */
  FMT_TRB,      /* oooooooooMmmmooo     BX   opcode Rhm */
  FMT_CPS,      /* 10110110011m0aif   MISC   CPSie/id */
  FMT_TR1RL8,   /* ooooonnnllllllll   MISC   opcode Rln, {Rl...} */
  FMT_TPCRL8,   /* oooooooRllllllll   MISC   opcode {Rl...[,PC]} */
  FMT_TLRRL8,   /* oooooooRllllllll   MISC   opcode {Rl...[,LR]} */
  FMT_TSETE,    /* 101101100101E000   MISC   setend */
  FMT_TLI22,    /* 11110IIIIIIIIIII 111HHiiiiiiiiiii UB opcode imm22 */

  /* ARM Instruction formats */
  FMT_A,

};

/* instruction sets */
#define IA4   0x0001 /* ARM instructions (armv4) */
#define IA4T  0x0002 /* ARM instructions (armv4t) */
#define IA5T  0x0004 /* ARM instructions (armv5t/5te/5tej) */
#define IA5TE 0x0008 /* ARM instructions (armv5te/5tej) */
#define IA5TJ 0x0010 /* ARM instructions (armv5tej) */
#define IA6   0x0020 /* ARM instructions (armv6) */
#define IA6D  0x0040 /* ARM instruction  (Deprecated in armv6) */
#define IT4T  0x0080 /* Thumb instructions (armv4t)*/
#define IT5T  0x0100 /* Thumb instructions (armv5t)*/
#define IT6   0x0200 /* Thumb instructions (armv6)*/
#define IT2   0x0400 /* Thumb-2 instructions (armv6m)*/

#define COUNT(tab) (sizeof(tab)/sizeof(tab[0]))

/*****************************************************************************
 * Types
 *****************************************************************************/

struct arm_operand_s
{
  uint16_t type;     /*recognized types*/
  uint8_t  reg  : 4; /*recognized register (main)*/
  uint8_t  regd : 4; /*recognized register (displacement)*/
  uint32_t value;    /*immediate value, label, or reg list */
};

/*****************************************************************************/
/* arm instructions */

struct arm_inst
{
  char *name;
  uint16_t isa; /* instruction set */
  uint16_t format; /* instruction format from arm_format_e */
  uint16_t ilen;   /* number of bytes in instruction */
  uint32_t opcode; /* bits with fixed values */
};

/*****************************************************************************/
/* forward declarations */

int arm_getinfos(struct asm_backend_infos_s *infos);
int arm_directive(const struct asm_backend_s *backend, struct asm_state_s *state, char *buf);
int arm_instruction(const struct asm_backend_s *backend, struct asm_state_s *state, char *buf);
int arm_option(const struct asm_backend_s *backend, struct asm_state_s *state, char *buf);

/*****************************************************************************/

const struct asm_backend_s arm_backend = 
{
  arm_getinfos,
  arm_directive,
  arm_instruction,
  arm_option,
};

/*****************************************************************************
 * Variables
 *****************************************************************************/

/* ARM7TDMI THUMB instructions */

struct arm_inst arm_thumb_instructions[] = /* DDI 0100i */
{
#include "arm_inst_thumb.h"
#include "arm_inst_code32.h"
};

/*****************************************************************************
 * Functions
 *****************************************************************************/

int arm_getinfos(struct asm_backend_infos_s *infos)
{
  infos->name = "arm";
  infos->endianess = ASM_ENDIAN_LITTLE;
  infos->wordsize = 4; /* 32-bit int and longs */
  infos->align_p2 = 1; /* align boundaries to power of twos */
  return ASM_OK;
}


/*****************************************************************************/

/* https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html */

int arm_option(const struct asm_backend_s *backend, struct asm_state_s *state, char *buf)
{
  printf("arm option: %s\n",buf);
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


/*****************************************************************************/

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

/*****************************************************************************/

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
      printf("-> %s\n",arm_thumb_instructions[i].name);
    }

  return ASM_OK;
}

