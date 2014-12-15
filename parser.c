#include "config.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "tcasm.h"

/*****************************************************************************/

static int parse_label(struct asm_state_s *state, char *label)
{
  printf("label [%s]\n", label);
  if (label[0] >= '0' && label[0]<='9')
    {
      return emit_message(state, ASM_ERROR, "invalid label '%s'",label);
    }
  return ASM_OK;
}

/*****************************************************************************/

static int parse_inst(struct asm_state_s *state, char *inst)
{
  printf("inst  [%s]\n", inst);
  return ASM_OK;
}

/*****************************************************************************/

static int parse_directive(struct asm_state_s *state, char *dir)
{
  char *params;
  int  ret = ASM_ERROR;

  /* split directive params */
  params = dir;
  while (*params && !(*params == ' ' || *params=='\t')) params++;
  *params = 0;
  params++;
  while (*params && (*params == ' ' || *params=='\t')) params++;

  printf("direc [%s]\n", dir);
  if(*params) printf("params [%s]\n", params);

  return directive(state, dir, params);
}

/*****************************************************************************/

static int parse_line(struct asm_state_s *state, int linelen)
{
  char *line = state->inbuf;
  char *label;
  char *mnemo;
  int  ret = ASM_OK;

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
      return ASM_OK;
    }

  /* discard LINE comments */

  if (*line==CONFIG_ASM_COMMENT_LINE)
    {
      return ASM_OK;
    }

  /* get first token */

printf("\nline  %s\n",line);

  label = line;
  while ( *line && (*line != ':') )
    {
      line++;
    }

  /* decide if that was a label */

  if (*line != ':')
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


  /* if the line has a CONTINUATION comment, cut the mnemo before that */

  line = mnemo;
  while(*line && *line != CONFIG_ASM_COMMENT_CONT) line++;
  *line = 0;

  /* right trim spaces after mnemonic */
  line--;
  while (line != mnemo && (*line==' ' || *line=='\t') )
    {
      *line=0;
      line--;
    }

  /* parse elements */
  if (label)
    {
      ret = parse_label(state, label);
    }

  if (ret != ASM_ERROR)
    {
      if (mnemo && strlen(mnemo))
        {
          if (mnemo[0]=='.')
            {
              ret = parse_directive(state, mnemo);
            }
          else
            {
              ret = parse_inst(state, mnemo);
            }
        }
    }

  return ret;
}

/*****************************************************************************/

int parse(struct asm_state_s *state)
{
  char *p;
  int  l;
  int  toolong; /* set to one if line was too long and end of line must be discarded */
  int  ret = ASM_OK;
  printf("-> %s\n", state->inputname);
  state->input = fopen(state->inputname, "rb");
  if (state->input==NULL)
    {
      printf("Cannot open '%s', errno=%d\n",state->inputname,errno);
      return ASM_ERROR;
    }
  state->curline = 0;

  while(1)
    {
      toolong = 0;
      p = fgets(state->inbuf, sizeof(state->inbuf), state->input);
      if(!p) break;
      state->curline += 1;
      l = strlen(p);
      toolong = (p[l-1] != '\n');
      ret = parse_line(state,l);
      if (ret == ASM_ERROR)
        {
          goto done;
        }
      /* if line was too long, eat the rest of the line until its end */
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

