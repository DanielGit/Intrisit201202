file _noahos.elf
target remote 192.168.1.125:6666

#########################################
# init gpio for sdram
#########################################
#GPIO_PAFUNS = 0xffffffff;
#GPIO_PASELC = 0xffffffff;
#GPIO_PAPES = 0xffffffff;
set {unsigned long}0xB0010044=0xffffffff	
set {unsigned long}0xB0010058=0xffffffff	
set {unsigned long}0xB0010034=0xffffffff	

#GPIO_PBFUNS = 0x81f9ffff;
#GPIO_PBSELC = 0x81f9ffff;
#GPIO_PBPES = 0x81f9ffff;
set {unsigned long}0xB0010144=0x81f9ffff	
set {unsigned long}0xB0010158=0x81f9ffff	
set {unsigned long}0xB0010134=0x81f9ffff
	
#GPIO_PCFUNS = 0x07000000;
#GPIO_PCSELC = 0x07000000;
#GPIO_PCPES = 0x07000000;
set {unsigned long}0xB0010244=0x07000000	
set {unsigned long}0xB0010258=0x07000000	
set {unsigned long}0xB0010234=0x07000000	

#########################################
# init pll
#########################################
#CPM_UHCCDR =
set {unsigned long}0xB000006c=0x00000003	
#PLL frequency division register
set {unsigned long}0xB0000000=0x40033330	
#PLL control register
set {unsigned long}0xB0000010=0x1b000520	

#########################################
# init sdram
#########################################
# REG_EMC_RTCSR = 0x0000;
# REG_EMC_RTCOR = 0x0000;
# REG_EMC_RTCNT = 0x0000;
set {unsigned short}0xB3010084=0x0000	
set {unsigned short}0xB301008c=0x0000	
set {unsigned short}0xB3010088=0x0000
	
# REG_EMC_DMCR = 0x042A120D;
# REG_EMC_RTCOR = 0x0019;
# REG_EMC_RTCSR = 0x0003;
# REG8(EMC_SDMR0|0x22) = 0x00;
set {unsigned long}0xB3010080=0x042A120D	
set {unsigned short}0xB301008c=0x0019	
set {unsigned short}0xB3010084=0x0003	
set {unsigned char}0xB301a022=0x00	

# REG_EMC_DMCR =	0x05AA120D;
# REG8(EMC_SDMR0|0x22) = 0;
set {unsigned long}0xB3010080=0x05AA120D	
set {unsigned char}0xB301a022=0	

load

#########################################
# init sdram
#########################################
break KernelMain
c
