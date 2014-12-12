#ifndef __TCASM__H__
#define __TCASM__H__

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

#endif /* __TCASM__H__ */

