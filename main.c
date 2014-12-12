/*
 * ARM assembler for embedded platforms such as NuttX
 * Copyright (c) 2014 Sebastien Lorquet
 * goal : save memory
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>

/*****************************************************************************
 * Config Definitions
 *****************************************************************************/

/* Maximum length of section name */
#ifndef CONFIG_ASM_SEC_NAME
#define CONFIG_ASM_SEC_NAME 8
#endif

/* Maximum number of sections */
#ifndef CONFIG_ASM_SEC_MAX
#define CONFIG_ASM_SEC_MAX 5 /*text, rodata, data, bss, strings*/
#endif

/* Current target */
#ifndef CONFIG_ASM_TARGET
#define CONFIG_ASM_TARGET CONFIG_ASM_TARGET_ARM
#endif

/* Input file buffer size */
#ifndef CONFIG_ASM_INBUF_SIZE
#define CONFIG_ASM_INBUF_SIZE 80
#endif

/* Line Comment char */
#ifndef CONFIG_ASM_COMMENT
#define CONFIG_ASM_COMMENT '#'
#endif

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/* Possible targets */
#define CONFIG_ASM_TARGET_ARM "arm"

enum section_names_e
{
  SECTION_STRINGS, /* this section is used as a string table */
  SECTION_TEXT,    /* this section stores exec code */
  SECTION_RODATA,  /* this section stores constant data */
  SECTION_DATA,    /* this section stores initialized data */
  SECTION_BSS,     /* this section stores uninitialized data */
  SECTION_CUSTOM   /* this is a custom section */
};

/*****************************************************************************
 * Types
 *****************************************************************************/

/* This structure is a chained list of binary data blocks to store assembled instructions.
 * Also used to store strings, such as symbol names.
 */

struct asm_chunk_s
{
  struct asm_chunk_s *next;
  int len;
  uint8_t *data;
};

/*****************************************************************************/
/* This structure is a relocation. All relocs are stored in a section*/

struct asm_reloc_s
{
  struct asm_reloc_s  *next;   /*these are chained */
  uint32_t            offset;  /* section offset where the relocation must be set */
  uint32_t            type;    /* type (PC relative, absolute, etc */
  struct asm_symbol_s *symbol; /* symbol reference */
};

/*****************************************************************************/
/* This structure is an output section. It has a name and contains code/data */

struct asm_section_s
{
  int  sec_id; /* fast section identification */
  char name[CONFIG_ASM_SEC_NAME]; /* section name */
  struct asm_chunk_s *data; /* section contents */
  struct asm_reloc_s *relocs;
};

/*****************************************************************************/
/* This structure is a symbol. It has a pointer to a name and a value. */

struct asm_symbol_s
{
  char                 *name;            /* pointer to an entry in the string table section */
  struct asm_section_s *holding_section; /* section in which the symbol is defined */
  uint32_t value;                        /* memory offset of the symbol within its section */
};


/*****************************************************************************/

/* this structure stores the entirety of all asm variables */

struct asm_state_s
{
  /* options */
  char *outputname; /* output file name */

  /* input status */
  FILE *input; /* currently managed input file */
  char inbuf[CONFIG_ASM_INBUF_SIZE]; /*buffer for reading input file */

  /* intermediate state */
  struct asm_section_s asm_sections[CONFIG_ASM_SEC_MAX]; /* storage for sections */

  /* output status */
  FILE *output; /* output file */

};

/*****************************************************************************
 * Variables
 *****************************************************************************/

struct asm_state_s state;

/*****************************************************************************
 * Functions
 *****************************************************************************/

int usage(void)
{
  printf("Mini assembler\n"
         "Current target: " CONFIG_ASM_TARGET "\n"
         "asm [options] infile [infile...]\n"
         "  -o <outfile> (default: <infile>.s)\n"
        );
  return 0;
}

/*****************************************************************************/

void init(struct asm_state_s *asmstate)
{
  asmstate->outputname = NULL;
}

/*****************************************************************************/

int assemble_line(struct asm_state_s *state, int linelen)
{
  char *line = state->inbuf;

  /* remove crlf */

  if(line[linelen-1]=='\n')
    {
      line[linelen-1] = 0;
      linelen -= 1;
    }

  if(line[linelen-1]=='\r')
    {
      line[linelen-1] = 0;
      linelen -= 1;
    }

  /* trim start spaces */

  while ( (*line == ' ') || (*line == '\t') )
    {
      line++;
      linelen--;
    }

  if (linelen==0)
    {
      return;
    }

  /* discard comments */
  if (*line==CONFIG_ASM_COMMENT)
    {
      return;
    }

  printf("[%s]\n",state->inbuf);
}

int assemble(struct asm_state_s *state, const char *filename)
{
  char *p;
  int  l;
  int  toolong; /* set to one if line was too long and end of line must be discarded */
  printf("-> %s\n", filename);
  state->input = fopen(filename, "rb");
  if (state->input==NULL)
    {
      printf("Cannot open input file, errno=%d\n",errno);
      return 1;
    }

  while(1)
    {
      toolong = 0;
      p = fgets(state->inbuf, sizeof(state->inbuf), state->input);
      if(!p) break;
      l = strlen(p);
      toolong = (p[l-1] != '\n');
      assemble_line(state,l);
      while (toolong)
        {
          p = fgets(state->inbuf, sizeof(state->inbuf), state->input);
          if(!p) break;
          l = strlen(p);
          toolong = (p[l-1] != '\n');
        }
    }

done:
  fclose(state->input);
  return 0;
}

/*****************************************************************************/

int main(int argc, char **argv)
{
  int option;
  int index;
  int ret = 0;

  /* Initialize the assembler state */

  init(&state);

  /* Update the assembler state using options */

  while ((option = getopt(argc, argv, "o:")) != -1)
    {
      if (option == 'o')
        {
          state.outputname = strdup(optarg);
        }
      else
        {
          usage();
          return 1;
        }
    }

  /* stop if there is no input file */

  if (optind == argc)
    {
      printf("tcasm: no input files\n");
      goto done;
    }

  /* Parse each input file */

  for(index=optind;index<argc;index++)
    {
      ret = assemble(&state, argv[index]);
      if (ret != 0)
        {
          goto donefree;
        }
    }

  /* resolve symbols after all files have been parsed */

  /* Define output file name if none was given */

  if (!state.outputname)
    {
      if (optind == argc-1)
        {
          state.outputname = strdup(argv[optind]);
          state.outputname[strlen(state.outputname)-1] = 'o';
        }
      else
        {
          state.outputname = strdup("a.out");
        }
    }

  printf("Output file name: %s\n",state.outputname);

  /* Write output file */

  /* Cleanup */

donefree:
  free(state.outputname);
done:
  return ret;
}
