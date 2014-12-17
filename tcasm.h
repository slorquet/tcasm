#ifndef __TCASM__H__
#define __TCASM__H__

#include <stdio.h>
#include <stdint.h>

enum section_names_e
{
  SECTION_NONE,    /* no section yet*/
  SECTION_STRINGS, /* this section is used as a string table */
  SECTION_TEXT,    /* this section stores exec code */
  SECTION_RODATA,  /* this section stores constant data */
  SECTION_DATA,    /* this section stores initialized data */
  SECTION_BSS,     /* this section stores uninitialized data */
  SECTION_CUSTOM   /* this is a custom section */
};

enum asm_error_e
{
  ASM_OK,
  ASM_WARN,
  ASM_ERROR
};

enum asm_endian_e
{
  ASM_ENDIAN_UNDEF,
  ASM_ENDIAN_LITTLE,
  ASM_ENDIAN_BIG
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
/* This structure is a relocation, a symbol reference. Each section has relocs.*/

struct asm_reloc_s
{
  struct asm_reloc_s   *next;   /*these are chained */
  uint32_t             offset;  /* section offset where the relocation must be set */
  uint32_t             type;    /* type (PC relative, absolute, etc */
  char                 *symbolname; /* symbol reference */
};

/*****************************************************************************/
/* This structure is an output section. It has a name and contains code/data */

struct asm_section_s
{
  int  id; /* fast section identification */
  char name[CONFIG_ASM_SEC_NAME]; /* section name */
  struct asm_chunk_s *data; /* section contents */
  struct asm_reloc_s *relocs; /*undefined symbols*/
};

/*****************************************************************************/
/* This structure is a DEFINED symbol (label). */

struct asm_symbol_s
{
  char     *name; /* pointer to an entry in the string table section */
  uint32_t value; /* memory offset of the symbol within its section */
};

/*****************************************************************************/

/* this structure describes a target backend */

struct asm_backend_infos_s
{
  char *name;
  int endianess;
};

struct asm_backend_s
{
  int (*getinfos)(struct asm_backend_infos_s *infos);
  int (*directive)(const struct asm_backend_s *backend, struct asm_state_s *state, char *buf);
};

/*****************************************************************************/

/* this structure stores the entirety of all asm variables */

struct asm_state_s
{
  /* options */
  char *outputname; /* output file name */

  /* input status */
  char *inputname; /* name of the current input file */
  FILE *input; /* currently managed input file */
  char inbuf[CONFIG_ASM_INBUF_SIZE]; /*buffer for reading input file */
  int  curline; /* current source line being read */

  /* intermediate state */
  struct asm_section_s sections[CONFIG_ASM_SEC_MAX]; /* storage for sections */
  struct asm_section_s *current_section;
  struct asm_backend_s *current_backend;

  /* output status */
  FILE *output; /* output file */

};

/*****************************************************************************/



/* parse a source file into the state */

int emit_message(struct asm_state_s *asmstate, int type, const char *msg, ...);

int parse(struct asm_state_s *state);
int parse_section(struct asm_state_s *state, const char *secname);

int directive(struct asm_state_s *state, char *dir, char *params);

struct asm_section_s *section_find_create(struct asm_state_s *asmstate, const char *secname);

int chunk_append(struct asm_state_s *state, struct asm_chunk_s **chlist, void *base, int len);
int chunk_append_block(struct asm_state_s *state, struct asm_chunk_s **chlist, void *base, int len);

#endif /* __TCASM__H__ */

