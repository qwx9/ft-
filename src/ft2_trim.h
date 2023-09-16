#ifndef _ft2_trim_h_
#define _ft2_trim_h_

void drawTrimScreen(void);
void hideTrimScreen(void);
void showTrimScreen(void);
void toggleTrimScreen(void);
void setInitialTrimFlags(void);
void trimThreadDone(void);
void resetTrimSizes(void);
void cbTrimUnusedPatt(void);
void cbTrimUnusedInst(void);
void cbTrimUnusedSamp(void);
void cbTrimUnusedChans(void);
void cbTrimUnusedSmpData(void);
void cbTrimSmpsTo8Bit(void);
void pbTrimCalc(void);
void pbTrimDoTrim(void);

#endif
