#include "config.h"
#include <stdlib.h>
#include <string.h>

#include "tcasm.h"

/* Append data to chunk, possibly splitting data in multiple blocks
 * The chunk list may be modified. Always succeed if there is enough memory.
 */

int chunk_append(struct asm_state_s *state, struct asm_chunk_s **chlist, void *base, int len)
{
  struct asm_chunk_s *ch = *chlist;
  struct asm_chunk_s *prev = NULL;
  int copy;

  /* seek to end of chain */

  while (ch) 
    {
      prev = ch;
      ch = ch->next;
    }

  /* now ch is null and prev is the last chunk, or null if the chain is empty */

  if (prev)
    {
      /* the chain has a chunk */
      printf("we have a chunk with %d bytes free\n", CONFIG_ASM_CHUNK - prev->len);
    }
  else
    {
      /* the chain is empty, create the first chunk */
      prev = malloc(CONFIG_ASM_CHUNK+sizeof(struct asm_chunk_s));
      if (!prev)
        {
          return emit_message(state, ASM_ERROR, "malloc() failed");
        }
      prev->data = (unsigned char*)(&prev[1]);
      prev->len  = 0;
      prev->next = NULL;
      *chlist = prev; /* we have allocated the first block of the chain */
      printf("made new chunk\n");
    }

  /* copy as many data as possible */
  copy = len;
  if (copy > (CONFIG_ASM_CHUNK - prev->len))
    {
      copy = CONFIG_ASM_CHUNK - prev->len;
    }
  memcpy(prev->data + prev->len, base, copy);
  prev->len += copy;
  printf("copied %d bytes, remaining in chunk:%d\n",len, (CONFIG_ASM_CHUNK - prev->len));
  base += copy;
  len -= copy;

  /* copy remaining data */

  while (len > 0)
    {
    copy = len;
    if (copy > (CONFIG_ASM_CHUNK - prev->len))
      {
        copy = CONFIG_ASM_CHUNK - prev->len;
      }
    printf("still %d to copy, prev can hold %d\n",len, copy);
    if (copy==0)
      {
        printf("new chunk\n");
        copy = CONFIG_ASM_CHUNK;
      }
    printf("copied: %d\n", copy);
    len -= copy;
    }
}

/* Append data to chunk NOT splitting it. Used for symbol strings.
 * The chunk list may be modified. 
 * May fail if the block cannot fit the maximum chunk size.
 */

int chunk_append_block(struct asm_state_s *state, struct asm_chunk_s **chlist, void *base, int len)
{
}
