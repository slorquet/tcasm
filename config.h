#ifndef __CONFIG__H__
#define __CONFIG__H__

/*****************************************************************************
 * TCASM Config Definitions
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

#endif /* __CONFIG__H__ */

