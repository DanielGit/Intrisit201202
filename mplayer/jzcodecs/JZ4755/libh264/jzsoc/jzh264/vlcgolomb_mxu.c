
#include "jzmedia.h"
#include "jzsoc/jzasm.h"
#include "bitstream.h"

static inline int get_ue_golomb_mxu(GetBitContext *gb){
    unsigned int rlt1 = 0, rlt2 = 0;
    int count=0,val;

    rlt1 = (unsigned int)gb->index>>3;
    rlt2 = (unsigned int)gb->index&7;
    
    S8LDD(xr1, gb->buffer+rlt1,0,ptn3);
    S8LDD(xr1, gb->buffer+rlt1,1,ptn2);
    S8LDD(xr1, gb->buffer+rlt1,2,ptn1);
    S8LDD(xr1, gb->buffer+rlt1,3,ptn0);

    D32SLLV(xr1,xr1,rlt2);
    val = S32M2I(xr1);
    count = i_clz(val);

    gb->index += count + 1;
    if (count == 0){
        return 0;
    } else {
        rlt1 = (unsigned int)(gb->index) >>3;
        rlt2 = (unsigned int)(gb->index) & 7;
  
        unsigned int gb_down_aligned, gb_ofst;
        gb_down_aligned = ((unsigned int)(gb->buffer+rlt1)) & (~3);
        gb_ofst = ( ((unsigned int)((gb->buffer+rlt1)) & 3) << 3 ) + rlt2;
  
        S32LDDR(xr2, gb_down_aligned, 0x0);
        S32LDDR(xr1, gb_down_aligned, 0x4);
        S32EXTRV(xr2, xr1, gb_ofst, count);
    
        val = S32M2I(xr2);
        gb->index += count;
        rlt1 = (1<<count) + val -1;
        return rlt1; 
   }
}

static inline int get_se_golomb_mxu(GetBitContext *gb){
    unsigned int rlt1 = 0, rlt2 = 0;
    int count=0, val;
    rlt1 = (unsigned int)gb->index>>3;
    rlt2 = (unsigned int)gb->index&7;
    
    S8LDD(xr1, gb->buffer+rlt1,0,ptn3);
    S8LDD(xr1, gb->buffer+rlt1,1,ptn2);
    S8LDD(xr1, gb->buffer+rlt1,2,ptn1);
    S8LDD(xr1, gb->buffer+rlt1,3,ptn0);
    D32SLLV(xr1,xr1,rlt2);
  
    val = S32M2I(xr1);
    count = i_clz(val);
    gb->index += count + 1;
    if (count == 0){
        return 0;
    } else {
        rlt1 = (unsigned int)(gb->index) >>3;
        rlt2 = (unsigned int)(gb->index) & 7;
  
        unsigned int gb_down_aligned, gb_ofst;
        gb_down_aligned = ((unsigned int)(gb->buffer+rlt1)) & (~3);
        gb_ofst = ( ((unsigned int)((gb->buffer+rlt1)) & 3) << 3 ) + rlt2;
  
        S32LDDR(xr2, gb_down_aligned, 0x0);
        S32LDDR(xr1, gb_down_aligned, 0x4);
        S32EXTRV(xr2, xr1, gb_ofst, count);
    
        val = S32M2I(xr2);
        gb->index += count;
        count = (1<<count) + val -1;
        if(count&1) count= ((count+1)>>1);
        else      count=  -(count>>1);
        return count;
    }
}

static inline int get_te0_golomb_mxu(GetBitContext *gb, int range){
    assert(range >= 1);

    if(range==1)      return 0;
    else if(range==2) return get_bits1(gb)^1;
    else              return get_ue_golomb_mxu(gb);
}

#define MXU_MEMSET(addr,data,len)   \
    do {                            \
       int32_t mxu_i;               \
       int32_t local = (int32_t)(addr)-4;  \
       int cline = ((len)>>5);		   \
       S32I2M(xr1, data);                  \
       for (mxu_i=0; mxu_i < cline; mxu_i++) \
       {                                     \
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

static inline void align_get_bits_mxu(GetBitContext *s)
{
    s->index += ((-(s->index))&7);
}

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

__h264_cavlc_tcsm_text__
static int get_vlc2_mxu(GetBitContext *s, VLC_TYPE (*table)[2],
                                  int bits, int max_depth)
{
    unsigned int rlt1, rlt2, code;
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
