//[PROPERTY]===========================================[PROPERTY]
//            *****  诺亚神舟操作系统V2  *****
//        --------------------------------------
//    	           MIDI音频播放函数部分
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2009-5      Init.             taoqh
//[PROPERTY]===========================================[PROPERTY]

#ifndef _DECODEC_MIDI_H
#define _DECODEC_MIDI_H

#define READ_MIDI_FILE_SIZE	512
#define MAX_MIDI_QUEUE_EVENTS		16
#define SPECIAL_PROGRAM -1

/* Midi events */
#define ME_NONE 				0
#define ME_NOTEON				1
#define ME_NOTEOFF				2
#define ME_KEYPRESSURE			3
#define ME_MAINVOLUME			4
#define ME_PAN					5
#define ME_SUSTAIN				6
#define ME_EXPRESSION			7
#define ME_PITCHWHEEL			8
#define ME_PROGRAM				9
#define ME_TEMPO				10
#define ME_PITCH_SENS			11

#define ME_ALL_SOUNDS_OFF		12
#define ME_RESET_CONTROLLERS	13
#define ME_ALL_NOTES_OFF		14
#define ME_TONE_BANK			15

#define ME_LYRIC				16

#define ME_EOT					99

/* Causes the instrument's default panning to be used. */
#define NO_PANNING -1

#define ISDRUMCHANNEL(hMidi,c) ((hMidi->drumchannels & (1<<(c))))

/* Voice status options: */
#define VOICE_FREE 0
#define VOICE_ON 1
#define VOICE_SUSTAINED 2
#define VOICE_OFF 3
#define VOICE_DIE 4

/* Voice panned options: */
#define PANNED_MYSTERY 0
#define PANNED_LEFT 1
#define PANNED_RIGHT 2
#define PANNED_CENTER 3

/* Bits in modes: */
#define MODES_16BIT	(1<<0)
#define MODES_UNSIGNED	(1<<1)
#define MODES_LOOPING	(1<<2)
#define MODES_PINGPONG	(1<<3)
#define MODES_REVERSE	(1<<4)
#define MODES_SUSTAIN	(1<<5)
#define MODES_ENVELOPE	(1<<6)

#ifdef LOOKUP_SINE
extern float sine(int x);
#else
#include <math.h>
#define sine(x) (sin((2*3.1415926/1024.0) * (x)))
#endif

#define SINE_CYCLE_LENGTH 1024

typedef struct {
  int32
    loop_start, loop_end, data_length,
    sample_rate, low_freq, high_freq, root_freq;
  int32
    envelope_rate[6], envelope_offset[6];
  float
    volume;
  sample_t *data;
  int32 
    tremolo_sweep_increment, tremolo_phase_increment, 
    vibrato_sweep_increment, vibrato_control_ratio;
  uint8
    tremolo_depth, vibrato_depth,
    modes;
  int8
    panning, note_to_use;
} Sample;

typedef struct {
  int samples;
  Sample *sample;
} Instrument;

/*
typedef struct {
  char *name;
  Instrument *instrument;
  int note, amp, pan, strip_loop, strip_envelope, strip_tail;
} ToneBankElement;
*/
/* A hack to delay instrument loading until after reading the
   entire MIDI file. */
#define MAGIC_LOAD_INSTRUMENT ((Instrument *)(-1))

typedef struct {
  Instrument *instrument[128];
} ToneBank;

typedef struct {
  int32 time;
  uint8 channel, type, a, b;
} MidiEvent;

typedef struct {
  MidiEvent event;
  void *next;
} MidiEventList;

typedef struct {
  int
    bank, program, volume, sustain, panning, pitchbend, expression, 
    mono, /* one note only on this channel -- not implemented yet */
    pitchsens;
  /* chorus, reverb... Coming soon to a 300-MHz, eight-way superscalar
     processor near you */
  float
    pitchfactor; /* precomputed pitch bend factor to save some fdiv's */
} Channel;

typedef struct {
  uint8
    status, channel, note, velocity;
  Sample *sample;
  int32
    orig_frequency, frequency,
    sample_offset, sample_increment,
    envelope_volume, envelope_target, envelope_increment,
    tremolo_sweep, tremolo_sweep_position,
    tremolo_phase, tremolo_phase_increment,
    vibrato_sweep, vibrato_sweep_position;
  
  final_volume_t left_mix, right_mix;

  float
    left_amp, right_amp, tremolo_volume;
  int32
    vibrato_sample_increment[VIBRATO_SAMPLE_INCREMENTS];
  int
    vibrato_phase, vibrato_control_ratio, vibrato_control_counter,
    envelope_stage, control_counter, panning, panned;

} Voice;

typedef struct _CHANNEL
{
	unsigned char percussion;
	unsigned char bank;
	unsigned char instrument;
	unsigned char reserved;
}CHANNEL;


typedef struct _MIDI_DECODEC
{
//midiinst.c
	/* Some functions get aggravated if not even the standard banks are 
	available. */
	ToneBank *tonebank[128];
	ToneBank *drumset[128];

	/* This is a special instrument, used for all melodic programs */
	Instrument *default_instrument;
	
	/* This is only used for tracks that don't specify a program */
	int default_program;

//midirsmp.c
	/* So it isn't interpolation. At least it's final. */
	sample_t resample_buffer[AUDIO_BUFFER_SIZE];

	void* (*MidiMalloc)(unsigned long size);
	void* (*MidiFree)(void* mem);

//midiread.c
	unsigned long quietchannels;

	int (*midi_callback)(FHND file,unsigned char* buf, int rsize);
	HANDLE midi_file;
	unsigned char midi_buf[READ_MIDI_FILE_SIZE];
	int midi_buf_remain;
	unsigned char *pmidi_buf;

	MidiEventList *evlist;
	int32 event_count;
	int32 at;

//midiplay.c
	Channel channel[16];
	Voice voice[MAX_VOICES];
	int voices;
	int32 control_ratio;
	int32 amplification;
	float master_volume;
	int adjust_panning_immediately;

	int32 lost_notes, cut_notes;
	int32 common_buffer[AUDIO_BUFFER_SIZE*2];	 /* stereo samples */
	int32 *buffer_pointer;
	int32 buffered_count;
	int32 drumchannels;
	int   midiquit;

	MidiEvent *event_list, *current_event;
	int32 sample_count, current_sample;
	int32 midi_count,midi_count_flag;
	void (*midi_pcm_callback)(HANDLE hMidi,void *id, void *buf, int bytes);

	MidiEvent midi_queue[MAX_MIDI_QUEUE_EVENTS];
	MidiEvent *midi_queue_head, *midi_queue_tail;
	int midi_run_time;

//mididrv.c
	BYTE* pMidiPcmBuf;
	DWORD nMidiPcmSize;
	MidiEvent *event;
}MIDI_DECODEC;
typedef MIDI_DECODEC *PMIDI_DECODEC;

extern HANDLE MidiDrvCrteate();
extern void MidiDrvDestory(HANDLE hMidi);

//midiinst.c
extern int load_instruments(PMIDI_DECODEC hMidi,char *wtb);
extern int load_usr_instruments(PMIDI_DECODEC hMidi,int drum, int bank, int tone);
extern void free_instruments(PMIDI_DECODEC hMidi);
extern int set_default_instrument(PMIDI_DECODEC hMidi,char *wtb, int dr, int bank, int tone);
extern int init_instruments(PMIDI_DECODEC hMidi,char *wtb);

//midiread.c
extern MidiEvent *read_midi_file(PMIDI_DECODEC hMidi ,int32 *count, int32 *sp);
extern void free_midi_file(PMIDI_DECODEC hMidi ,MidiEvent *evlist);
extern int get_midi_file_time(PMIDI_DECODEC hMidi ,FHND file, void* cb);
extern void midi_quit(PMIDI_DECODEC hMidi );

//midiplay.c
extern int play_midi(PMIDI_DECODEC hMidi ,MidiEvent *el, int32 events, int32 samples);
extern int send_midi_event(PMIDI_DECODEC hMidi ,MidiEvent *event);
extern int run_midi(PMIDI_DECODEC hMidi ,char *ch);
extern void init_midi_play(PMIDI_DECODEC hMidi,MidiEvent *eventlist, int32 events, int32 samples);
extern int get_midi_pcm(PMIDI_DECODEC hMidi);

//midimix.c
extern void mix_voice(PMIDI_DECODEC hMidi,int32 *buf, int v, int32 c);
extern int recompute_envelope(PMIDI_DECODEC hMidi,int v);
extern void apply_envelope_to_amp(PMIDI_DECODEC hMidi,int v);

//midirsmp.c
extern sample_t *resample_voice(PMIDI_DECODEC hMidi ,int v, int32 *countptr);

//miditbls.c
extern int32 freq_table[];
extern float vol_table[];
extern float bend_fine[];
extern float bend_coarse[];
#endif
