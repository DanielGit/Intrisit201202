

#include "common.h"
#include "faad.h"

extern faacDecHandle faac_hdec;
extern faacDecFrameInfo faac_finfo;

void* NEAACDECAPI mtv_faacDecDecode(NeAACDecFrameInfo *hInfo,
                                 uint8_t *buffer, uint32_t buffer_size)
{
    unsigned char *pcm_o;
    pcm_o = faacDecDecode(faac_hdec, &faac_finfo, buffer, buffer_size);            //decode a audio packet
    *hInfo = faac_finfo;
    return (void *)pcm_o;
}

