
#ifndef AO_JZ4740_VOL_H
#define AO_JZ4740_VOL_H
#include "jz_mxu.h"


void replay_fill_all_16_mid(void *block,int count,void *outbuf,int jz_audio_volume,int shift);
void replay_fill_all_8_mid(void *block,int count,void *outbuf,int jz_audio_volume,int shift);

#endif //AO_JZ4740_VOL_H
