/*
 * The file define all the common macro for the board based on the JZ4760
 */


#ifndef __JZ4760_COMMON_H__
#define __JZ4760_COMMON_H__


#define __CFG_EXTAL     (OSC_CLOCK / 1000000)
#define __CFG_PLL_OUT   (CFG_CPU_SPEED / 1000000)
 
#if (__CFG_PLL_OUT > 1500)
	#error "PLL output can NOT more than 1500"
#elif (__CFG_PLL_OUT >= 624)
	#define __PLL_OD          0
	#define __PLL_NO          1
#elif (__CFG_PLL_OUT >= 336)
        #define __PLL_OD          1
        #define __PLL_NO          2
#elif (__CFG_PLL_OUT >= 168)
        #define __PLL_OD          2
        #define __PLL_NO          4
#elif (__CFG_PLL_OUT >= 72)
        #define __PLL_OD          3
        #define __PLL_NO          8
#else
	#error "PLL ouptput can NOT less than 72"
#endif

#define __PLL_N		2
#define __PLL_M		(((__CFG_PLL_OUT / __CFG_EXTAL) * __PLL_NO * __PLL_N) >> 1)


#if ((__PLL_M > 127) || (__PLL_M < 2))
	#error "Can NOT set the value to PLL_M"
#endif


#define CPCCR_M_N_OD	((__PLL_M << 24) | (__PLL_N << 18) | (__PLL_OD << 16))


#endif /* __JZ4760_COMMON_H__ */

