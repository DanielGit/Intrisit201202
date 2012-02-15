#ifndef _BURN_H
#define _BURN_H

#if defined(STC_EXP)
int sBurnLoad(const char *mac, const char *idexfile, const char *datafile);
void sBurnUnload(void);
void sBurnRun(void);
int sBurnIsCompleted(void);
int sBurnGetStage(int *percent, int *cur, int *totals);
BYTE *sBurnGetMsg(BYTE *outstr, int *len);
#else
int BurnLoad(const char *mac, const char *idexfile, const char *datafile);
void BurnUnload(void);
void BurnRun(void);
int BurnIsCompleted(void);
int BurnGetStage(int *percent, int *cur, int *totals);
BYTE *BurnGetMsg(BYTE *outstr, int *len);
#endif
#endif
