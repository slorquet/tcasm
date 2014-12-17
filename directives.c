#include "config.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "tcasm.h"

/*****************************************************************************/

static int parse_section(struct asm_state_s *state, const char *secname)
{
  printf("section [%s]\n", secname);
  state->current_section = section_find_create(state, secname);

  return ASM_OK;
}

/*****************************************************************************/

/* .ds / .space <size> [, <fillbyte>] */
static int parse_space(struct asm_state_s *state, const char *params)
{
  uint8_t buf[8];
  uint32_t size,step;
  char *rest;
  uint8_t fill = 0;

  printf("space ->%s\n", params);

  /* eat spaces */
  while (*params && (*params==' ' || *params=='\t')) params++;
  printf("after ->'%s'\n", params);
  if (!params)
    {
    return emit_message(state, ASM_ERROR, "bad space directive");
    }
  size = strtol(params, &rest, 0);
  params = rest;
  printf("size: %d\n",size);
  /* eat spaces */
  while (*params && (*params==' ' || *params=='\t' || *params==',')) params++;

  printf("after ->'%s'\n", params);
  if(*params)
    {
    fill = strtol(params, &rest, 0);
    }
  printf("fill: %u\n",fill);

  /* do the fill */

  memset(buf, fill, 8);
  while (size>0)
    {
      step = size;
      if (step>8) step = 8;
      chunk_append(state, &state->current_section->data, buf, step);
      size -= step;
    }
 
  return ASM_OK;
}

/*****************************************************************************/

/* Append a string to the current section, possibly adding a final zero. */

static int directive_cb_append_string(struct asm_state_s *state, char **str, int arg)
{
  char *base = *str;
  //skip to end of string
  if (*base!='\"')
    {
      return emit_message(state, ASM_ERROR, "Invalid string litteral, expected \"", *base);
    }
  base++;
  (*str)++;
  while (**str && **str!='"') (*str)++;
  **str=0;
  (*str)++; /* ready for next param */

  printf("in section [%s] append string %s'%s'\n",state->current_section->name, arg?"with zeros ":"", base);
  chunk_append(state, &state->current_section->data, base, strlen(base));
  if (arg)
    {
      chunk_append(state, &state->current_section->data, "", 1);
    }
  return ASM_OK;
}

/*****************************************************************************/

static void number_encode(uint8_t *dest, int val, int size, int endianess)
{
  if (size<1 || size>4)
    {
      return;
    }
  if (endianess == ASM_ENDIAN_LITTLE)
    {
      dest[0]     = (val      ) & 0xFF;
      if (size>1)
        {
          dest[1] = (val >>  8) & 0xFF;
        }
      if (size>2)
        {
          dest[2] = (val >> 16) & 0xFF;
        }
      if (size>3)
        {
          dest[3] = (val >> 24) & 0xFF;
        }
    }
  else if (endianess == ASM_ENDIAN_BIG)
    {
      int off = 0;
      if (size>3)
        {
          dest[off++] = (val >> 24) & 0xFF;
        }
      if (size>2)
        {
          dest[off++] = (val >> 16) & 0xFF;
        }
      if (size>1)
        {
          dest[off++] = (val >>  8) & 0xFF;
        }
      dest[off++]     = (val      ) & 0xFF;
    }
  else
    {
      /* undefined endianess */
    }
}

/*****************************************************************************/

/* Append a number to the current section. arg is number of bytes */

static int directive_cb_append_number(struct asm_state_s *state, char **str, int arg)
{
  long val;
  char *rest;
  uint8_t encoded[4];
  struct asm_backend_infos_s info;
  val = strtol(*str, &rest, 0);

  /*printf("parsed val=%ld, after number : %s\n",val, rest);*/

  /* if what follows is not a sep, then we have garbage */
  if ( *rest && !(*rest==' ' || *rest=='\t' || *rest==',') )
    {
      return emit_message(state, ASM_ERROR, "Invalid number: near %s", *str);
    }

  /* eat all non-seps */
  while(*rest && !(*rest==' ' || *rest=='\t' || *rest==',')) rest++;

  *str = rest;

  /* add to current section */
  state->current_backend->getinfos(&info);
  number_encode(encoded, val, arg, info.endianess);
  chunk_append(state, &state->current_section->data, encoded, arg);

  return ASM_OK;
}

/*****************************************************************************/

/* excute the provided callback for each comma-separated value in param, passing arg to each call. */

int directive_for_each_param(struct asm_state_s *state, char *params, int (*callback)(struct asm_state_s *state, char **param, int arg), int arg)
{
  int ret;
  while (*params)
    {
      while(*params && (*params==' ' || *params=='\t' || *params==',')) params++;
      ret = callback(state, &params, arg);
      if (ret == ASM_ERROR)
        {
          return ret;
        }
    }
    return ASM_OK;
}

/*****************************************************************************/

/* manage directives */

int directive(struct asm_state_s *state, char *dir, char *params)
{
  int ret;
  if (!strcmp(dir, ".section"))
    {
      char *ptr = params;
      /* find end of section name */
      while (*ptr && !(*ptr == ' ' || *ptr=='\t')) ptr++;
      *ptr=0;
      ret = parse_section(state, params);
    }
  else if (!strcmp(dir, ".text") || !strcmp(dir, ".data") || !strcmp(dir, ".bss") || !strcmp(dir, ".rodata") )
    {
      ret = parse_section(state, dir);
    }
  else if (!strcmp(dir, ".db") || !strcmp(dir, ".byte") )
    {
      ret = directive_for_each_param(state, params, directive_cb_append_number, 1);
    }
  else if (!strcmp(dir, ".dh") || !strcmp(dir, ".hword") || !strcmp(dir, ".short")  )
    {
      ret = directive_for_each_param(state, params, directive_cb_append_number, 2);
    }
  else if (!strcmp(dir, ".ds") || !strcmp(dir, ".space") )
    {
      ret = parse_space(state, params);
    }
  else if (!strcmp(dir, ".ascii") || !strcmp(dir, ".string") || !strcmp(dir, ".asciz") )
    {
      ret = directive_for_each_param(state, params, directive_cb_append_string, !strcmp(dir, ".asciz"));
    }
  else
    {
    ret = emit_message(state, ASM_WARN, "unknown directive '%s'", dir);
    }
  return ret;
}

