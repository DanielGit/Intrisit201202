#ifndef __JZ4760AHB_H__
#define __JZ4760AHB_H__



#define AHB0_BASE  0xB3000000
#define PRIOR_OFFSET  	 0x00
#define CTRL_OFFSET      0x04
#define CLKL_OFFSET      0x08
#define EVENT0L_OFFSET   0x0c
#define EVENT1L_OFFSET   0x10
#define EVENTH_OFFSET    0x14
#define WATCHCTRL_OFFSET 0x18
#define WATCHADDR_OFFSET 0x1c
#define WATCHAMSK_OFFSET 0x20
#define WATCHDATA_OFFSET 0x24
#define WATCHDMAK_OFFSET 0x28 

#define AHB_PRIOR(n) (AHB0_BASE + (n*0x200000) + PRIOR_OFFSET )
#define AHB_CTRL(n)  (AHB0_BASE + (n*0x200000) + CTRL_OFFSET )
#define AHB_CLKL(n)  (AHB0_BASE + (n*0x200000) + CLKL_OFFSET )
#define AHB_EV0L(n)  (AHB0_BASE + (n*0x200000) + EVENT0L_OFFSET )
#define AHB_EV1L(n)	 (AHB0_BASE + (n*0x200000) + EVENT1L_OFFSET )
#define AHB_EVH(n)   (AHB0_BASE + (n*0x200000) + EVENTH_OFFSET )
#define AHB_WCTRL(n) (AHB0_BASE + (n*0x200000) + WATCHCTRL_OFFSET ) 
#define AHB_WADDR(n) (AHB0_BASE + (n*0x200000) + WATCHADDR_OFFSET )
#define AHB_WAMSK(n) (AHB0_BASE + (n*0x200000) + WATCHAMSK_OFFSET )
#define AHB_WDATA(n) (AHB0_BASE + (n*0x200000) + WATCHDATA_OFFSET ) 
#define AHB_WDMSK(n) (AHB0_BASE + (n*0x200000) + WATCHDMAK_OFFSET )


#define REG_AHB_PRIOR(n)  REG32(AHB_PRIOR(n))
#define REG_AHB_CTRL(n)   REG32(AHB_CTRL(n))
#define REG_AHB_CLKL(n)   REG32(AHB_CLKL(n)) 
#define REG_AHB_EV0L(n)   REG32(AHB_EV0L(n))
#define REG_AHB_EV1L(n)   REG32(AHB_EV1L(n))
#define REG_AHB_EVH(n)    REG32(AHB_EVH(n))
#define REG_AHB_WCTRL(n)  REG32(AHB_WCTRL(n))
#define REG_AHB_WADDR(n)  REG32(AHB_WADDR(n))
#define REG_AHB_WAMSK(n)  REG32(AHB_WAMSK(n))
#define REG_AHB_WDATA(n)  REG32(AHB_WDATA(n))
#define REG_AHB_WDMSK(n)  REG32(AHB_WDMSK(n))


#define M1(n)   ((n << 20))
#define M0(n)   ((n << 16))
#define EV0(n)  ((n << 12))
#define EV1(n)  ((n << 8))
#define EV1E_EN (1 << 2) 
#define EV1E_DE (~(1<<2))
#define EV0E_EN (1 << 1)
#define EV0E_DE (~(1<<1))
#define CLKE_EN ( 1<< 0)
#define CLKE_DE (~(1<<0))



/*n : AHBn */

#define MONITOR_START(n,m0,ev0,m1,ev1)\
do{ \
		REG_AHB_CLKL(n) = 0x0; \
	  REG_AHB_EV0L(n) = 0x0; \
	  REG_AHB_EV1L(n) = 0x0; \
	  REG_AHB_EVH(n)  = 0x0; \
	  REG_AHB_CTRL(n) = (M0(m0)|EV0(ev0)|M1(m1)|EV1(ev1)|EV1E_EN|EV0E_EN|CLKE_EN);\
	}while(0) 

#define MONITOR_STOP(n) \
do{ \
	  REG_AHB_CTRL(n) = 0x0; \
		REG_AHB_CLKL(n) = 0x0; \
	  REG_AHB_EV0L(n) = 0x0; \
	  REG_AHB_EV1L(n) = 0x0; \
	  REG_AHB_EVH(n)  = 0x0; \
	}while(0)	
	
#define MONITOR_LOG(n) \
do{\
			unsigned int ev0 = REG_AHB_EV0L(n);\
			unsigned int ev1 = REG_AHB_EV1L(n);\
			unsigned int clk = REG_AHB_CLKL(n);\
			unsigned int high = REG_AHB_EVH(n);\
			printf(" ev0 %d ev1 %d clk %d ev0H %d ev1H %d clkH %d pev0  %d  pev1 %d \n", \
							ev0,ev1,clk,0xff&high,0xff&(high>>8),0xffff&(high>>16),ev0/(clk/100),ev1/(clk/100));\
	}while(0)	
#endif 

