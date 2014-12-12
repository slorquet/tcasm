/*
 * ARM assembler for embedded platforms such as NuttX
 * Copyright (c) 2014 Sebastien Lorquet
 * goal : save memory
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* Possible targets */
#define 
/* Configuration */

/* Maximum length of section name */
#ifndef CONFIG_ASM_SEC_NAME
#define CONFIG_ASM_SEC_NAME 8
#endif

/* Maximum number of sections */
#ifndef CONFIG_ASM_SEC_MAX
#define CONFIG_ASM_SEC_MAX 4 /*text, rodata, data, bss*/
#endif

/* Current target */
#ifndef CONFIG_ASM_TARGET
#define CONFIG_ASM_TARGET CONFIG_ASM_TARGET_ARM
#endif

/* This structure is a chained list of binary data blocks to store assembled instructions */

struct asm_chunk_s
{
  struct asm_chunk_s *next;
  int len;
  uint8_t *data;
};

/* This structure is an output section. It has a name and contains code */

struct asm_section_s
{
  char name[CONFIG_ASM_SEC_NAME]; /* section name */
  struct asm_chunk_s *data; /* section contents */
};

/* storage for sections */
struct asm_section_s asm_sections[CONFIG_ASM_SEC_MAX];

int usage(void)
{
  printf("Mini assembler\n"
         "Current target: " CONFIG_ASM_TARGET "\n"
         "asm [options] infile\n"
         "  -o <outfile> (default: <infile>.s)\n"
        );
  return error;
}

int main(int argc, char **argv)
{
}
