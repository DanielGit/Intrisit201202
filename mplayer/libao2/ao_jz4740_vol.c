#include "ao_jz4740_vol.h"


inline void replay_fill_all_16(short *inblock,int count,short *outblock,int jz_audio_volume,int shift)
{
	unsigned int endblock;
	//printf("%x %x",inblock,outblock);
	inblock -=  2;
	outblock -=  2;
        endblock = inblock + count / 2;	
	S32I2M(xr15,jz_audio_volume);
	//printf("mplayer vol control!\n");
	while((unsigned int)inblock < endblock) 
	{
	
		S32LDI(xr1,inblock,4);    
		S32LDI(xr2,inblock,4);    
		S32LDI(xr3,inblock,4);   
		S32LDI(xr4,inblock,4);    
   
		D16MUL_LW(xr5,xr15,xr1,xr6);
		D16MUL_LW(xr7,xr15,xr2,xr8);
		D16MUL_LW(xr9,xr15,xr3,xr10);
		D16MUL_LW(xr11,xr15,xr4,xr12);
		
		D32SARW(xr5,xr5,xr6,shift);
		D32SARW(xr7,xr7,xr8,shift);
		D32SARW(xr9,xr9,xr10,shift);
		D32SARW(xr11,xr11,xr12,shift);
	
		S32SDI(xr5,outblock,4);
		S32SDI(xr7,outblock,4);
		S32SDI(xr9,outblock,4);
		S32SDI(xr11,outblock,4);
		
	}
	//printf("%x %x",inblock,outblock);
	//while(1);
} 

inline void replay_fill_all_8(char *inblock,int count,char *outblock,int jz_audio_volume,int shift)
{
	unsigned int endblock;
	S32I2M(xr15,jz_audio_volume);
	S32SFL(xr0,xr15,xr15,xr15,ptn3);
	S32SFL(xr0,xr15,xr15,xr15,ptn1);
        inblock -= 4;
        outblock -= 4;
        endblock = inblock + count;
	while(inblock < endblock)
	{
		S32LDI(xr1,inblock,0x4);   //xr1:s[3] s[2] s[1] s[0]
		S32LDI(xr2,inblock,0x4);
		S32LDI(xr3,inblock,0x4);
		S32LDI(xr4,inblock,0x4);
    
		Q8MUL(xr5,xr1,xr15,xr6); //xr5:s[3]* s[2]* xr6:s[1]* s[0]*
		Q8MUL(xr7,xr2,xr15,xr8);
		Q8MUL(xr9,xr3,xr15,xr10);
		Q8MUL(xr11,xr4,xr15,xr12);
  
		Q16SARV(xr5,xr6,shift);
		Q16SARV(xr7,xr8,shift);
		Q16SARV(xr9,xr10,shift);
		Q16SARV(xr11,xr12,shift);

	        Q16SAT(xr6,xr5,xr6);	
                Q16SAT(xr8,xr7,xr8);
	        Q16SAT(xr10,xr9,xr10);
                Q16SAT(xr12,xr11,xr12);
		
                S32SDI(xr6,outblock,0x4);
		S32SDI(xr8,outblock,0x4);
		S32SDI(xr10,outblock,0x4);
		S32SDI(xr12,outblock,0x4);               
	}
}
void replay_fill_all_16_mid(void *block,int count,void *outbuf,int jz_audio_volume,int shift)
{
	replay_fill_all_16((short *)block,count,(short *)outbuf,jz_audio_volume,shift);
}

void replay_fill_all_8_mid(void *block,int count,void *outbuf,int jz_audio_volume,int shift)
{
	replay_fill_all_8((char *)block,count,(char *)outbuf,jz_audio_volume,shift);
}


