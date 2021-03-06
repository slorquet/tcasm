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

/* these definitions depend on the presence of backends */


#ifdef CONFIG_ASM_TARGET_ARM
extern struct asm_backend_s arm_backend;
#endif

/* preprocessor cannot use sizeof. so use an enum instead */
enum asm_backend_counter_e
{
#ifdef CONFIG_ASM_TARGET_ARM
  arm_backend_index,
#endif
  ASM_BACKEND_COUNT
};

static struct asm_backend_s * backends[] = 
{
#ifdef CONFIG_ASM_TARGET_ARM
  &arm_backend,
#endif
};

static struct asm_state_s state;

/*****************************************************************************
 * Functions
 *****************************************************************************/

void usage(void)
{
  printf("Tiny Compact Assembler\n"
         "tcasm [options] infile [infile...]\n"
         "  -I <path> Add dir to include path\n"
         "  -o <outfile> (default: <infile>.s, or a.out if multiple infiles)\n"
         "  -v version info\n");
  if(ASM_BACKEND_COUNT > 1)
    printf(
         "  -b <target> select backend\n"
         "  -m backend options (must appear after -b)\n");
  else
    printf(
         "  -m backend options\n");
}

/*****************************************************************************/

void version(void)
{
  int i;
  struct asm_backend_infos_s infos;
  printf("tcasm version " CONFIG_ASM_VERSION "\n" );
  printf("Configured backends:");
  for (i=0; i<ASM_BACKEND_COUNT; i++)
    {
      backends[i]->getinfos(&infos);
      printf(" %s", infos.name);
    }
  printf("\n");
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
  if (type > ASM_ERROR)
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
  asmstate->current_backend = NULL;
  for (i = 0; i<CONFIG_ASM_SEC_MAX; i++)
    {
      asmstate->sections[i].id = SECTION_NONE;
    }
  for (i = 0; i < CONFIG_ASM_INC_COUNT; i++)
    {
      asmstate->includes[i]=NULL;
    }
}

/*****************************************************************************/

int main(int argc, char **argv)
{
  int option;
  int index;
  char *asm_options;
  int ret = 0;

  struct asm_backend_infos_s infos;

  /* Initialize the assembler state */

  init(&state);

  /* Determine the correct backend */

  if (ASM_BACKEND_COUNT == 1)
    {
      state.current_backend = backends[0];
    }

  if (ASM_BACKEND_COUNT > 1)
    {
      asm_options = "bm:hI:o:v";
    }
  else
    {
      asm_options = "m:hI:o:v";
    }

  /* parse options */

  while ((option = getopt(argc, argv, asm_options)) != -1)
    {
      if (option == 'o')
        {
          state.outputname = strdup(optarg);
        }
      else if (option == 'I')
        {
          /* first, check that option length is reasonable */
          if (strlen(optarg) > CONFIG_ASM_INC_MAXLEN)
            {
              fprintf(stderr, "Include path too long\n");
              continue;
            }
          for (index = 0; index < CONFIG_ASM_INC_COUNT; index++)
            {
              if (state.includes[index]==NULL)
                {
                  state.includes[index] = optarg;
                  break;
                }
              else if(!strcmp(state.includes[index], optarg))
                {
                  break;
                }

            }
          if (index == CONFIG_ASM_INC_COUNT)
            {
              fprintf(stderr,"Error: too many includes, discarded '%s'\n",optarg);
            }
        }
      else if (option == 'h')
        {
          usage();
          return 0;
        }
      else if (option == 'v')
        {
          version();
          return 0;
        }
      else if (option == 'b')
        {
          int found = 0;
          /* list all backends, then select */
          for (index = 0; index < ASM_BACKEND_COUNT; index++)
            {
              backends[index]->getinfos(&infos);
              if (!strcmp(infos.name, optarg))
                {
                  state.current_backend = backends[index];
                  found = 1;
                  break;
                }
            }
          if (!found)
            {
              fprintf(stderr, "Unknown backend %s\n", optarg);
              return 1;
            }
        }
      else if (option == 'm')
        {
          if (!state.current_backend)
            {
              fprintf(stderr, "No backend selected\n");
              return 1;
            }
          ret = state.current_backend->option(state.current_backend, &state, optarg);
          if (ret != 0)
            {
              return ret;
            }
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

  /* stop if multiple backends are available and non was chosen */
  if (!state.current_backend)
    {
      printf("More than one backend available, choose with -b\n");
      return 1;
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
  /* For now we just dump the sections */
  for (index = 0; index < CONFIG_ASM_SEC_MAX; index++)
    {
      uint32_t i;
      uint32_t offset = 0;
      struct asm_chunk_s *chunk = state.sections[index].data;
      if(!chunk)
        {
          continue;
        }
      printf("Contents of section %s: %u bytes\n", state.sections[index].name, chunk_totalsize(state.sections[index].data) );
      while (chunk)
        {
          printf("chunk @ %p len %u\n", chunk, chunk->len);
          for (i = 0; i < chunk->len; i++, offset++)
            {
              if ((offset&15) == 0)
                {
                  printf("%08X: ", offset);
                }
              printf("%02X ", chunk->data[i]);
              if ((offset&15) == 15)
                {
                  printf("\n");
                }
            }
          if ((offset&15))
            {
              printf("\n");
            }
          chunk = chunk->next;
        }
    }

  /* Cleanup */

donefree:
  free(state.outputname);
  return ret;
}
