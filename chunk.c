#include "config.h"
#include <stdlib.h>
#include <string.h>

#include "tcasm.h"

#define DEBUG 0
#define DEBUG_CHUNK 1

/* Append data to chunk, possibly splitting data in multiple blocks.
 * The chunk list may be modified. Always succeed if there is enough memory.
 * This routine fills chunks to their maximum possible size.
 */

int chunk_append(struct asm_state_s *state, struct asm_chunk_s **chlist, void *base, int len)
{
  struct asm_chunk_s *ch = *chlist;
  struct asm_chunk_s *prev = NULL;
  int copy;

#if DEBUG & DEBUG_CHUNK
  printf("TODO: %d bytes\n",len);
#endif
  /* seek to end of chain */

  while (ch) 
    {
      prev = ch;
      ch = ch->next;
    }
  ch = prev;

  /* now ch is null and prev is the last chunk, or null if the chain is empty */

  if (!ch)
    {
      /* the chain is empty, create the first chunk */
      ch = malloc(CONFIG_ASM_CHUNK+sizeof(struct asm_chunk_s));
      if (!ch)
        {
          return emit_message(state, ASM_ERROR, "malloc() failed");
        }
      ch->data = (unsigned char*)(&ch[1]);
      ch->len  = 0;
      ch->next = NULL;
      *chlist = ch; /* we have allocated the first block of the chain */
#if DEBUG & DEBUG_CHUNK
      printf("made initial chunk\n");
#endif
    }
#if DEBUG & DEBUG_CHUNK
  else
    {
      /* the chain has a chunk */
      printf("we have a chunk with %d bytes free\n", CONFIG_ASM_CHUNK - prev->len);
    }
#endif

  /* copy as many data as possible */

  while (len > 0)
    {
      copy = len;
      if (copy > (CONFIG_ASM_CHUNK - ch->len))
        {
          copy = CONFIG_ASM_CHUNK - ch->len;
        }
#if DEBUG & DEBUG_CHUNK
      printf("cur chunk can contain: %d\n", CONFIG_ASM_CHUNK - ch->len);
#endif
      if (copy==0)
        {
          /* no room in current chunk */
#if DEBUG & DEBUG_CHUNK
          printf("new chunk required\n");
#endif
          prev = ch;
          ch = malloc(CONFIG_ASM_CHUNK+sizeof(struct asm_chunk_s));
          if (!ch)
            {
              return emit_message(state, ASM_ERROR, "malloc() failed");
            }
          ch->data = (unsigned char*)(&ch[1]);
          ch->len  = 0;
          ch->next = NULL;
          prev->next = ch;
          continue;
        }
#if DEBUG & DEBUG_CHUNK
      printf("total remaining %d, will store %d\n",len,copy);
#endif
      memcpy(ch->data + ch->len, base, copy);
      ch->len += copy;
#if DEBUG & DEBUG_CHUNK
      printf("copied %d bytes, remaining in chunk:%d\n", copy, (CONFIG_ASM_CHUNK - ch->len));
#endif
      base += copy;
      len -= copy;
    }

#if DEBUG & DEBUG_CHUNK
  printf("summary\n");
  ch = *chlist;
  while (ch)
    {
      printf("chunk @ %p len=%d\n",ch, ch->len);
      ch = ch->next;
    }
#endif
}

/* Append data to chunk NOT splitting it. Used for symbol strings.
 * The chunk list may be modified. 
 * May fail if the block cannot fit the maximum chunk size.
 * This routine may not fill chunks to their maximum possible size (if a block doesnt fit).
 */

int chunk_append_block(struct asm_state_s *state, struct asm_chunk_s **chlist, void *base, int len)
{
}

/* return the total size of a chunk list */

uint32_t chunk_totalsize(struct asm_chunk_s *chlist)
{
  uint32_t total = 0;
  while (chlist)
    {
      total += chlist->len;
      chlist = chlist->next;
    }
  return total;
}

