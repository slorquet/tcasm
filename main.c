/*
 * ARM assembler for embedded platforms such as NuttX
 * Copyright (c) 2014 Sebastien Lorquet
 * goal : save memory
 *
 */

#include "config.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "tcasm.h"

/*****************************************************************************
 * Definitions
 *****************************************************************************/

/*****************************************************************************
 * Types
 *****************************************************************************/

/*****************************************************************************
 * Variables
 *****************************************************************************/

static struct asm_state_s state;

/*****************************************************************************
 * Functions
 *****************************************************************************/

int usage(void)
{
  printf("Mini assembler\n"
         "tcasm [options] infile [infile...]\n"
         "  -o <outfile> (default: <infile>.s, or a.out if multiple infiles)\n"
         "  -l list targets and exit\n"
         "  -t <target> select target\n"
        );
  return 0;
}

/*****************************************************************************/

static const char * msgtypes[] = 
{
  "message",
  "warning",
  "error",
};

int emit_message(struct asm_state_s *asmstate, int type, const char *msg, ...)
{
  va_list ap;
  if (type > (sizeof(msgtypes)/sizeof(msgtypes[0])))
    {
    type = 0;
    }
  va_start(ap, msg);
  fprintf(stderr, "%s:%d: %s: ",asmstate->inputname, asmstate->curline, msgtypes[type]);
  vfprintf(stderr, msg, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  return type;
}

/*****************************************************************************/

void init(struct asm_state_s *asmstate)
{
  int i;
  asmstate->outputname = NULL;
  asmstate->current_section = NULL;
  for (i = 0; i<CONFIG_ASM_SEC_MAX; i++)
    {
      asmstate->sections[i].id = SECTION_NONE;
    }
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
      goto donefree;
    }

  /* Parse each input file */

  for(index=optind;index<argc;index++)
    {
      state.inputname = argv[index];
      ret = parse(&state);
      if (ret != 0)
        {
          goto donefree;
        }
    }

  /* linking stage : resolve symbols after all files have been parsed */

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
  return ret;
}
