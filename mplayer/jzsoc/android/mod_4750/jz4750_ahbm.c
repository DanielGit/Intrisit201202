#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#define REG32(addr)	*((volatile unsigned int *)(addr))
/* define macro for HARB */
#define	HARB_BASE	        0xB3000000

#define HARB_PRIOR              (HARB_BASE + 0x00)
#define HARB_CTRL               (HARB_BASE + 0x04) /* AHB monitor control */
#define HARB_CLKL               (HARB_BASE + 0x08) /* AHB clock counter low */
#define HARB_EVENT0L            (HARB_BASE + 0x0c) /* AHB bus event 0 counter low */
#define HARB_EVENT1L            (HARB_BASE + 0x10) /* AHB bus event 1 counter low */
#define HARB_EVENTH             (HARB_BASE + 0x14) /* AHB bus event & clock counter high */

#define REG_HARB_PRIOR          REG32(HARB_PRIOR)
#define REG_HARB_CTRL           REG32(HARB_CTRL)
#define REG_HARB_CLKL           REG32(HARB_CLKL)
#define REG_HARB_EVENT0L        REG32(HARB_EVENT0L)
#define REG_HARB_EVENT1L        REG32(HARB_EVENT1L)
#define REG_HARB_EVENTH         REG32(HARB_EVENTH)

#define HARB_EVENT_0            0// bus transaction cycles, exclude idle cycles
#define HARB_EVENT_1            1// bus transaction times, total NONSEQ times
#define HARB_EVENT_2            2// grant latency, total pending request cycles for occurred transctions 
#define HARB_EVENT_3            3// critical grant latency trigger
#define HARB_EVENT_4            4// single beat transaction times, BURST type is SINGLE
#define HARB_EVENT_5            5// fixed length burst transaction times,BURST type is INCR4/8/16 or WRAP4/8/16
#define HARB_EVENT_6            6// INCR bust transaction times, BURST type is INCR

#define HARB_EVENT_CPU          0
#define HARB_EVENT_CIM          1
#define HARB_EVENT_LCD          2
#define HARB_EVENT_DMA          3
#define HARB_EVENT_IPU          4
#define HARB_EVENT_USB          5
#define HARB_EVENT_VMC          6
#define HARB_EVENT_TFM          7
#define HARB_EVENT_ALL          15

#define HARB_CHANNEL0_EVENT HARB_EVENT_0
#define HARB_CHANNEL0_MASTER HARB_EVENT_CPU
#define HARB_CHANNEL1_EVENT HARB_EVENT_2
#define HARB_CHANNEL1_MASTER HARB_EVENT_IPU

const char *harb_event_string[]=
{
	"bus transaction cycles",
	"bus transaction times",
	"grant latency",
	"critical grant latency",
	"single beat transaction times",
	"fixed length burst transaction times",
	"INCR bust transaction times"
};

const char *harb_master_string[]=
{
	"cpu",
	"cim",
	"lcd",
	"dma",
	"ipu",
	"usb",
	"vmc",
	"tfm",
	"res",
	"res",
	"res",
	"res",
	"res",
	"res",
	"res",
	"all"
};

void ahb_start_monitor(unsigned char master)
{
	unsigned int ctrl = 0;
	ctrl = (HARB_CHANNEL0_EVENT << 8) | (master << 16);
	ctrl |= (HARB_CHANNEL1_EVENT << 12) | (master << 20);
	REG_HARB_CLKL = 0;
	REG_HARB_EVENT0L = 0;
	REG_HARB_EVENT1L = 0;
	REG_HARB_EVENTH = 0;
//	printf("REG_HARB_PRIOR = %x \n",REG_HARB_PRIOR);
	REG_HARB_PRIOR = 0x1;
	REG_HARB_CTRL = (ctrl | 0x7);
//	printf("REG_HARB_CTRL = %x \n",REG_HARB_CTRL);
}

void ahb_stop_monitor()
{
	REG_HARB_CTRL = 0;
}

void ahb_show_count()
{
	char evn0,evn1,mst0,mst1;
	unsigned long long count0,count1,clk;
	int per0,per1;

	count0 = (unsigned long long)(REG_HARB_EVENTH & 0xff) << 32 | REG_HARB_EVENT0L;
	count1 = (unsigned long long)(REG_HARB_EVENTH & 0xff00) << 24 | REG_HARB_EVENT1L;
	clk = (unsigned long long)(REG_HARB_EVENTH & 0xffff0000) << 16 | REG_HARB_CLKL;

	evn0 = (REG_HARB_CTRL >> 8) & 0x7;
	evn1 = (REG_HARB_CTRL >> 12) & 0x7;
	mst0 = (REG_HARB_CTRL >> 16) & 0xf;
	mst1 = (REG_HARB_CTRL >> 20) & 0xf;

	per0 = (count0 / 100) / (clk / 100000);
	per1 = (count1 / 100) / (clk / 100000);

//	printf("total clock count :%llx (%llu) \n\n", clk, clk);

//	printf("channel 0: \n");
//	printf("monitor event :%s \n", harb_event_string[evn0]);
	printf("monitor master:%s \n", harb_master_string[mst0]);
//	printf("channel count :%llx (%llu)\n", count0, count0);
//	printf("percent       :%d \n\n",per0);

//	printf("channel 1: \n");
//	printf("monitor event :%s \n", harb_event_string[evn1]);
//	printf("monitor master:%s \n", harb_master_string[mst1]);
//	printf("channel count :%llx (%llu)\n", count1, count1);
	printf("percent       :%d \n\n",per1);

}

unsigned int ahb_get_bus_clock(void)
{
	unsigned int ret = REG_HARB_CLKL;
	REG_HARB_CTRL = 0x1;
	REG_HARB_CLKL = 0;
	REG_HARB_PRIOR = 0x0;
	return ret / 120000;
}
