
int mp_get_mp3_header(unsigned char* hbuf,int* chans, int* freq, int* spf, int* mpa_layer, int* br);

#define mp_decode_mp3_header(hbuf)  mp_get_mp3_header(hbuf,NULL,NULL,NULL,NULL,NULL)

static inline int mp_check_mp3_header(unsigned int head)
{
	unsigned char buf[4];
    if( (head & 0x0000e0ff) != 0x0000e0ff ||  
        (head & 0x00fc0000) == 0x00fc0000) return 0;

	buf[3] = (unsigned char)((head >> 24) & 0xff);
	buf[2] = (unsigned char)((head >> 16) & 0xff);
	buf[1] = (unsigned char)((head >> 8) & 0xff);
	buf[0] = (unsigned char)((head >> 0) & 0xff);

    if(mp_decode_mp3_header(buf)<=0)
		return 0;
    return 1;
}
