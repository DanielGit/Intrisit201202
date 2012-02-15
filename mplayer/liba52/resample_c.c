// this code is based on a52dec/libao/audio_out_oss.c

static inline int16_t convert (int32_t i)
{
#ifndef LIBA52_FIXED
    if (i > 0x43c07fff)
	return 32767;
    else if (i < 0x43bf8000)
	return -32768;
    else
      return i - 0x43c00000;
#else
    i = (i >> 16);
    if(i == 0)
  	i = 1;
    return i;
#endif
}

static int a52_resample_MONO_to_5_C(float * _f, int16_t * s16){
    int i;
    int32_t * f = (int32_t *) _f;
	for (i = 0; i < 256; i++) {
	    s16[5*i] = s16[5*i+1] = s16[5*i+2] = s16[5*i+3] = 0;
	    s16[5*i+4] = convert (f[i]);
	}
    return 5*256;
}

static int a52_resample_MONO_to_1_C(float * _f, int16_t * s16){
    int i;
    int32_t * f = (int32_t *) _f;
	for (i = 0; i < 256; i++) {
	    s16[i] = convert (f[i]);
	}
    return 1*256;
}

static int a52_resample_STEREO_to_2_C(sample_t * _f, int16_t * s16){
    int i;
#if defined(JZ4750_OPT)
    int32_t * f = ((int32_t *) _f - 1);
    int32_t * f2 = f + 256;
    i = 256;
    s16-=2;
    do{
        S32LDI(xr1, f, 0x04);
        S32LDI(xr2, f2, 0x04);
        S32SFL(xr1,xr1,xr2,xr0,ptn3);
        S32SDI(xr1, s16, 0x04);
    }while(i--);
#else
    int32_t * f = (int32_t *) _f;
    int32_t * f2 = f + 256;
    //printf("%s\n",__FUNCTION__);
    for (i = 0; i < 256; i++) {
    	 
        s16[2*i] = convert (f[i]);
        s16[2*i+1] = convert (f2[i]);
    }
#endif
    return 2*256;
}

static int a52_resample_3F_to_5_C(float * _f, int16_t * s16){
    int i;
    int32_t * f = (int32_t *) _f;
	for (i = 0; i < 256; i++) {
	    s16[5*i] = convert (f[i]);
	    s16[5*i+1] = convert (f[i+512]);
	    s16[5*i+2] = s16[5*i+3] = 0;
	    s16[5*i+4] = convert (f[i+256]);
	}
    return 5*256;
}

static int a52_resample_2F_2R_to_4_C(float * _f, int16_t * s16){
    int i;
    int32_t * f = (int32_t *) _f;
	for (i = 0; i < 256; i++) {
	    s16[4*i] = convert (f[i]);
	    s16[4*i+1] = convert (f[i+256]);
	    s16[4*i+2] = convert (f[i+512]);
	    s16[4*i+3] = convert (f[i+768]);
	}
    return 4*256;
}

static int a52_resample_3F_2R_to_5_C(float * _f, int16_t * s16){
    int i;
    int32_t * f = (int32_t *) _f;
	for (i = 0; i < 256; i++) {
	    s16[5*i] = convert (f[i]);
	    s16[5*i+1] = convert (f[i+512]);
	    s16[5*i+2] = convert (f[i+768]);
	    s16[5*i+3] = convert (f[i+1024]);
	    s16[5*i+4] = convert (f[i+256]);
	}
    return 5*256;
}

static int a52_resample_MONO_LFE_to_6_C(float * _f, int16_t * s16){
    int i;
    int32_t * f = (int32_t *) _f;
	for (i = 0; i < 256; i++) {
	    s16[6*i] = s16[6*i+1] = s16[6*i+2] = s16[6*i+3] = 0;
	    s16[6*i+4] = convert (f[i+256]);
	    s16[6*i+5] = convert (f[i]);
	}
    return 6*256;
}

static int a52_resample_STEREO_LFE_to_6_C(float * _f, int16_t * s16){
    int i;
    int32_t * f = (int32_t *) _f;
	for (i = 0; i < 256; i++) {
	    s16[6*i] = convert (f[i+256]);
	    s16[6*i+1] = convert (f[i+512]);
	    s16[6*i+2] = s16[6*i+3] = s16[6*i+4] = 0;
	    s16[6*i+5] = convert (f[i]);
	}
    return 6*256;
}

static int a52_resample_3F_LFE_to_6_C(float * _f, int16_t * s16){
    int i;
    int32_t * f = (int32_t *) _f;
	for (i = 0; i < 256; i++) {
	    s16[6*i] = convert (f[i+256]);
	    s16[6*i+1] = convert (f[i+768]);
	    s16[6*i+2] = s16[6*i+3] = 0;
	    s16[6*i+4] = convert (f[i+512]);
	    s16[6*i+5] = convert (f[i]);
	}
    return 6*256;
}

static int a52_resample_2F_2R_LFE_to_6_C(float * _f, int16_t * s16){
    int i;
    int32_t * f = (int32_t *) _f;
	for (i = 0; i < 256; i++) {
	    s16[6*i] = convert (f[i+256]);
	    s16[6*i+1] = convert (f[i+512]);
	    s16[6*i+2] = convert (f[i+768]);
	    s16[6*i+3] = convert (f[i+1024]);
	    s16[6*i+4] = 0;
	    s16[6*i+5] = convert (f[i]);
	}
    return 6*256;
}

static int a52_resample_3F_2R_LFE_to_6_C(float * _f, int16_t * s16){
    int i;
    int32_t * f = (int32_t *) _f;
	for (i = 0; i < 256; i++) {
	    s16[6*i] = convert (f[i+256]);
	    s16[6*i+1] = convert (f[i+768]);
	    s16[6*i+2] = convert (f[i+1024]);
	    s16[6*i+3] = convert (f[i+1280]);
	    s16[6*i+4] = convert (f[i+512]);
	    s16[6*i+5] = convert (f[i]);
	}
    return 6*256;
}


static void* a52_resample_C(int flags, int ch){
    switch (flags) {
    case A52_MONO:
	if(ch==5) return a52_resample_MONO_to_5_C;
	if(ch==1) return a52_resample_MONO_to_1_C;
	break;
    case A52_CHANNEL:
    case A52_STEREO:
    case A52_DOLBY:
	if(ch==2) return a52_resample_STEREO_to_2_C;
	break;
    case A52_3F:
	if(ch==5) return a52_resample_3F_to_5_C;
	break;
    case A52_2F2R:
	if(ch==4) return a52_resample_2F_2R_to_4_C;
	break;
    case A52_3F2R:
	if(ch==5) return a52_resample_3F_2R_to_5_C;
	break;
    case A52_MONO | A52_LFE:
	if(ch==6) return a52_resample_MONO_LFE_to_6_C;
	break;
    case A52_CHANNEL | A52_LFE:
    case A52_STEREO | A52_LFE:
    case A52_DOLBY | A52_LFE:
	if(ch==6) return a52_resample_STEREO_LFE_to_6_C;
	break;
    case A52_3F | A52_LFE:
	if(ch==6) return a52_resample_3F_LFE_to_6_C;
	break;
    case A52_2F2R | A52_LFE:
	if(ch==6) return a52_resample_2F_2R_LFE_to_6_C;
	break;
    case A52_3F2R | A52_LFE:
	if(ch==6) return a52_resample_3F_2R_LFE_to_6_C;
	break;
    }
    return NULL;
}
