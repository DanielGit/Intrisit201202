/*
 * sub_cc.c - Decoder for Closed Captions
 *
 * This decoder relies on MPlayer's OSD to display subtitles.
 * Be warned that the decoding is somewhat preliminary, though it basically works.
 * 
 * Most notably, only the text information is decoded as of now, discarding color,
 * background and position info (see source below).
 * 
 * by Matteo Giani
 *
 * uses source from the xine closed captions decoder
 */


#ifdef __LINUX__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else
#include <mplaylib.h>
#include "mplaylib.h"
#endif


#include "config.h"
#include "sub_cc.h"

#include "subreader.h"

#include "libvo/video_out.h"
#include "libvo/sub.h"


#define CC_MAX_LINE_LENGTH 64

static char chartbl[128];

static subtitle buf1,buf2;
static subtitle *fb,*bb;

static unsigned int cursor_pos=0;

static int inited=0;

static void build_char_table(void)
{
  int i;
  /* first the normal ASCII codes */
  for (i = 0; i < 128; i++)
    chartbl[i] = (char) i;
  /* now the special codes */
  chartbl[0x2a] = '?';
  chartbl[0x5c] = '?';
  chartbl[0x5e] = '?';
  chartbl[0x5f] = '?';
  chartbl[0x60] = '?';
  chartbl[0x7b] = '?';
  chartbl[0x7c] = '?';
  chartbl[0x7d] = '?';
  chartbl[0x7e] = '?';
  chartbl[0x7f] = '?';    /* FIXME: this should be a solid block */
}

static void clear_buffer(subtitle *buf)
{
	int i;
	buf->lines=0;
	for(i=0;i<SUB_MAX_TEXT;i++) if(buf->text[i]) {free(buf->text[i]);buf->text[i]=NULL;}
}


void subcc_init(void)
{
	int i;
	//printf("subcc_init(): initing...\n");
	build_char_table();
	for(i=0;i<SUB_MAX_TEXT;i++) {buf1.text[i]=buf2.text[i]=NULL;}
	buf1.lines=buf2.lines=0;
	fb=&buf1;
	bb=&buf2;
	
	inited=1;
}

static void append_char(char c)
{
	if(!bb->lines) {bb->lines++; cursor_pos=0;}
	if(bb->text[bb->lines - 1]==NULL) 
	{
		bb->text[bb->lines - 1]=malloc(CC_MAX_LINE_LENGTH);
		memset(bb->text[bb->lines - 1],0,CC_MAX_LINE_LENGTH);
		cursor_pos=0;
	}
	
	if(c=='\n')
	{
		if(cursor_pos>0 && bb->lines < SUB_MAX_TEXT)
			bb->lines++;cursor_pos=0;
	}
	else 
	{
		if(cursor_pos==CC_MAX_LINE_LENGTH-1)
		{
			fprintf(stderr,"sub_cc.c: append_char() reached CC_MAX_LINE_LENGTH!\n");
			return;
		}
		bb->text[bb->lines - 1][cursor_pos++]=c;
	}
}


static void swap_buffers(void)
{
	subtitle *foo;
	foo=fb;
	fb=bb;
	bb=foo;
}

static void display_buffer(subtitle * buf)
{
	vo_sub=buf;
	vo_osd_changed(OSDTYPE_SUBTITLE);
}


static void cc_decode_EIA608(unsigned short int data)
{
  
  static unsigned short int lastcode=0x0000;	
  unsigned char c1 = data & 0x7f;
  unsigned char c2 = (data >> 8) & 0x7f;

  if (c1 & 0x60) {		/* normal character, 0x20 <= c1 <= 0x7f */
	   append_char(chartbl[c1]);
	   if(c2 & 0x60)	/*c2 might not be a normal char even if c1 is*/
		   append_char(chartbl[c2]);
  }
  else if (c1 & 0x10)		// control code / special char
  {
//	  int channel= (c1 & 0x08) >> 3;
	  c1&=~0x08;
	  if(data!=lastcode)
	  {
	  	if(c2 & 0x40) {	/*PAC, Preamble Address Code */
			append_char('\n'); /*FIXME properly interpret PACs*/
		}
		else
			switch(c1)
			{
				case 0x10:	break; // ext attribute
				case 0x11: 
					if((c2 & 0x30)==0x30) 
					{
						//printf("[debug]:Special char (ignored)\n");
						/*cc_decode_special_char()*/;
					}
					else if (c2 & 0x20) 
					{
						//printf("[debug]: midrow_attr (ignored)\n");
						/*cc_decode_midrow_attr()*/;
					}
					break;
				case 0x14:
					switch(c2)
					{
						case 0x2C: display_buffer(NULL); //EDM
							   clear_buffer(fb); break;
						case 0x2d: append_char('\n');	//carriage return
							   break;
						case 0x2e: clear_buffer(bb);	//ENM
							   break;
						case 0x2f: swap_buffers();	//Swap buffers
							   display_buffer(fb);
							   clear_buffer(bb);
							   break;
					}
					break;
				case 0x17:
					if( c2>=0x21 && c2<=0x23) //TAB
					{
						break;
					}
			}
	  }
  } 
  lastcode=data;  
}

static void subcc_decode(unsigned char *inputbuffer, unsigned int inputlength)
{
  /* The first number may denote a channel number. I don't have the
   * EIA-708 standard, so it is hard to say.
   * From what I could figure out so far, the general format seems to be:
   *
   * repeat
   *
   *   0xfe starts 2 byte sequence of unknown purpose. It might denote
   *        field #2 in line 21 of the VBI. We'll ignore it for the
   *        time being.
   *
   *   0xff starts 2 byte EIA-608 sequence, field #1 in line 21 of the VBI.
   *        Followed by a 3-code triplet that starts either with 0xff or
   *        0xfe. In either case, the following triplet needs to be ignored
   *        for line 21, field 1.
   *
   *   0x00 is padding, followed by 2 more 0x00.
   *
   *   0x01 always seems to appear at the beginning, always seems to
   *        be followed by 0xf8, 8-bit number. 
   *        The lower 7 bits of this 8-bit number seem to denote the
   *        number of code triplets that follow.
   *        The most significant bit denotes whether the Line 21 field 1 
   *        captioning information is at odd or even triplet offsets from this
   *        beginning triplet. 1 denotes odd offsets, 0 denotes even offsets.
   *      
   *        Most captions are encoded with odd offsets, so this is what we
   *        will assume.
   *
   * until end of packet
   */
  unsigned char *current = inputbuffer;
  unsigned int curbytes = 0;
  unsigned char data1, data2;
  unsigned char cc_code;
  int odd_offset = 1;

  while (curbytes < inputlength) {
    int skip = 2;

    cc_code = *(current);

    if (inputlength - curbytes < 2) {
#ifdef LOG_DEBUG
      fprintf(stderr, "Not enough data for 2-byte CC encoding\n");
#endif
      break;
    }
    
    data1 = *(current+1);
    data2 = *(current + 2);
    current++; curbytes++;
    
    switch (cc_code) {
    case 0xfe:
      /* expect 2 byte encoding (perhaps CC3, CC4?) */
      /* ignore for time being */
      skip = 2;
      break;
      
    case 0xff:
      /* expect EIA-608 CC1/CC2 encoding */
      // FIXME check parity!
      // Parity check omitted assuming we are reading from a DVD and therefore
      // we should encounter no "transmission errors".
      cc_decode_EIA608(data1 | (data2 << 8));
      skip = 5;
      break;
      
    case 0x00:
      /* This seems to be just padding */
      skip = 2;
      break;
      
    case 0x01:
      odd_offset = data2 & 0x80;
      if (odd_offset)
	skip = 2;
      else
	skip = 5;
      break;
      
    default:
//#ifdef LOG_DEBUG
      fprintf(stderr, "Unknown CC encoding: %x\n", cc_code);
//#endif
      skip = 2;
      break;
    }
    current += skip;
    curbytes += skip;
  }
}


void subcc_process_data(unsigned char *inputdata,unsigned int len)
{
	if(!subcc_enabled) return;
	if(!inited) subcc_init();
	
	subcc_decode(inputdata, len);
}

