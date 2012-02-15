#include "jzmedia.h"
#include "jzasm.h"
#include "bitstream.h"
#include "vc1_aux.h"
#define MXU_MEMSET(addr,data,len)   \
    do {                            \
       int32_t mxu_i;               \
       int32_t local = (int32_t)(addr)-4;  \
       int cline = ((len)>>5);		   \
       S32I2M(xr1, data);                  \
       for (mxu_i=0; mxu_i < cline; mxu_i++) \
       {                                     \
	 i_pref(30,local,4);		     \
	   S32SDI(xr1,local,4);              \
           S32SDI(xr1,local,4);              \
           S32SDI(xr1,local,4);              \
           S32SDI(xr1,local,4);              \
           S32SDI(xr1,local,4);              \
           S32SDI(xr1,local,4);              \
           S32SDI(xr1,local,4);              \    
           S32SDI(xr1,local,4);              \
       }                                     \
    }while(0)

#define JZ_DCACHE_ALLOC 30
//__vc1_J1_tcsm_text__
static void block_memset_MXU(const int16_t *data){
  uint32_t i;
  uint32_t block = (uint32_t)data;
  for(i=0;i<4;i++){
    i_pref(JZ_DCACHE_ALLOC, block, 0);
    S32STD(xr0, block, 0);
    S32STD(xr0, block, 4);
    S32STD(xr0, block, 8);
    S32STD(xr0, block, 12);
    S32STD(xr0, block, 16);
    S32STD(xr0, block, 20);
    S32STD(xr0, block, 24);
    S32STD(xr0, block, 28);
    block += 32;
  }
}
 

//__vc1_J1_tcsm_text__
static inline unsigned int get_bits_mxu(GetBitContext *s, int n){
    register tmp;
    unsigned int pbs = (unsigned int)(s->index) >>3;
    unsigned int rbs = (unsigned int)(s->index) & 7; 
    unsigned int gb_down_aligned, gb_ofst;
    gb_down_aligned = ((unsigned int)(s->buffer+pbs)) & (~3);
    gb_ofst = ( ((unsigned int)((s->buffer+pbs)) & 3) << 3 ) + rbs;

    S32LDDR(xr2, gb_down_aligned, 0x0);
    S32LDDR(xr1, gb_down_aligned, 0x4);
    S32EXTRV(xr2, xr1, gb_ofst, n);
    tmp = S32M2I(xr2);
    s->index += n;
    return tmp;
}

static inline int get_vlc2_mxu(GetBitContext *s, VLC_TYPE (*table)[2],
                                  int bits, int max_depth)
{
    unsigned int rlt1, rlt2, code, pref_down;
    int n, val, nb_bits;
    rlt1 = (unsigned int)(s->index) >>3;
    rlt2 = (unsigned int)(s->index) & 7;
  
    unsigned int gb_down_aligned, gb_ofst;
    gb_down_aligned = ((unsigned int)(s->buffer+rlt1)) & (~3);
    gb_ofst = ( ((unsigned int)((s->buffer+rlt1)) & 3) << 3 ) + rlt2;
  
    S32LDDR(xr2, gb_down_aligned, 0x0);
    S32LDDR(xr1, gb_down_aligned, 0x4);
    S32EXTRV(xr2, xr1, gb_ofst, bits);
    val = S32M2I(xr2);                 //SHOW_UBITS
    pref_down = ((unsigned int)(&table[val][0])) & (~31);
    i_pref(0, pref_down, 0);
    code = table[val][0];
    n    = table[val][1];

    if(max_depth > 1 && n < 0){
        s->index += bits;
        nb_bits = -n;
        rlt1 = (unsigned int)(s->index) >>3;
        rlt2 = (unsigned int)(s->index) & 7;

 	gb_down_aligned = ((unsigned int)(s->buffer+rlt1)) & (~3);
	gb_ofst = ( ((unsigned int)((s->buffer+rlt1)) & 3) << 3 ) + rlt2;
  
        S32LDDR(xr2, gb_down_aligned, 0x0);
	S32LDDR(xr1, gb_down_aligned, 0x4);
        S32EXTRV(xr2, xr1, gb_ofst, nb_bits);
        val =S32M2I(xr2);
        val += code;

        code = table[val][0];
        n    = table[val][1];
        if(max_depth > 2 && n < 0){
            s->index += nb_bits;
            nb_bits = -n;
            rlt1 = (unsigned int)(s->index) >>3;
            rlt2 = (unsigned int)(s->index) & 7;

            gb_down_aligned = ((unsigned int)(s->buffer+rlt1)) & (~3);
            gb_ofst = ( ((unsigned int)((s->buffer+rlt1)) & 3) << 3 ) + rlt2;
  
            S32LDDR(xr2, gb_down_aligned, 0x0);
            S32LDDR(xr1, gb_down_aligned, 0x4);
            S32EXTRV(xr2, xr1, gb_ofst, nb_bits);
            val = S32M2I(xr2);
            val += code;
                
            code = table[val][0];
            n    = table[val][1];
        }
    }
    s->index += n;
    return code;
}
