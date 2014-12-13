#include "config.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "tcasm.h"

/*****************************************************************************/

int parse_label(struct asm_state_s *state, char *label)
{
  printf("label [%s]\n", label);
  return 0;
}

/*****************************************************************************/

int parse_mnemo(struct asm_state_s *state, char *mnemo)
{
  printf("inst  [%s]\n", mnemo);
  return 0;
}

/*****************************************************************************/

int parse_line(struct asm_state_s *state, int linelen)
{
  char *line = state->inbuf;
  char *label;
  char *mnemo;

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

  /* get first token */

  label = line;
  while ( *line && (*line != ' ') && (*line != '\t') )
    {
      line++;
    }

  /* decide if that was a label */

  if (*(line-1) != ':')
    {
      /* Not a label */
      mnemo = label;
      label = NULL;
    }
  else
    {
      /* that was a label.
       * skip spaces and find mnemo */
      *line=0;
      line++;
      while ( *line && ((*line == ' ') || (*line == '\t')) )
        {
          line++;
        }

      mnemo = line;
    }


  /* if the line has a comment, cut the mnemo before that */

  line = mnemo;
  while(*line && *line != CONFIG_ASM_COMMENT) line++;
  *line = 0;

  if (label)
    {
      parse_label(state, label);
    }

  if (mnemo && strlen(mnemo))
    {
      parse_mnemo(state, mnemo);
    }

  return 0;
}

/*****************************************************************************/

int parse(struct asm_state_s *state, const char *filename)
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
      parse_line(state,l);
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
