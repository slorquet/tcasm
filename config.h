#ifndef __CONFIG__H__
#define __CONFIG__H__

/*****************************************************************************
 * TCASM Config Definitions
 *****************************************************************************/

/* Program version */
#define CONFIG_ASM_VERSION "0.01"

/* Configured targets */
#define CONFIG_ASM_TARGET_ARM 1

/* Maximum length of section name */
#ifndef CONFIG_ASM_SEC_NAME
#define CONFIG_ASM_SEC_NAME 8
#endif

/* Maximum number of sections */
#ifndef CONFIG_ASM_SEC_MAX
#define CONFIG_ASM_SEC_MAX 5 /*text, rodata, data, bss, strings*/
#endif


/* Input file buffer size */
#ifndef CONFIG_ASM_INBUF_SIZE
#define CONFIG_ASM_INBUF_SIZE 80
#endif

/* Line Comment char */
#ifndef CONFIG_ASM_COMMENT_LINE
#define CONFIG_ASM_COMMENT_LINE '#'
#endif

/* Continuation Comment char */
#ifndef CONFIG_ASM_COMMENT_CONT
#define CONFIG_ASM_COMMENT_CONT '@'
#endif

/* Allocation chunk size */
#ifndef CONFIG_ASM_CHUNK
#define CONFIG_ASM_CHUNK 256
#endif

/* Maximum number of include path entries */
#ifndef CONFIG_ASM_INC_COUNT
#define CONFIG_ASM_INC_COUNT 4
#endif

/* Maximum length of include paths. Sanity check only */
#ifndef CONFIG_ASM_INC_MAXLEN
#define CONFIG_ASM_INC_MAXLEN 80
#endif

#endif /* __CONFIG__H__ */

