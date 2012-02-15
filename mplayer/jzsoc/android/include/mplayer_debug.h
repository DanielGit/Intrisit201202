#ifndef __MPLAYER_DEBUG_H__
#define __MPLAYER_DEBUG_H__

#define DEBUG_LEVEL_VERBOSE 6
#define DEBUG_LEVEL_DBG 	5
#define DEBUG_LEVEL_INFO 	4
#define DEBUG_LEVEL_MSG 	3
#define DEBUG_LEVEL_WARN 	2
#define DEBUG_LEVEL_ERR 	1
#define DEBUG_LEVEL_NOTHING 0
#define DEBUG_LEVEL_ENTER 	DEBUG_LEVEL_INFO
#define DEBUG_LEVEL_LEAVE 	DEBUG_LEVEL_INFO




#define MPLAYER_DEBUG_LEVEL DEBUG_LEVEL_MSG /* debug level */


#if MPLAYER_DEBUG_LEVEL >= DEBUG_LEVEL_DBG
#define MPLAYER_DBG()													\
	do {																\
		printf("%s[%d], %s()::\n", __FILE__, __LINE__, __FUNCTION__);	\
	}while(0)

#else
#define MPLAYER_DBG() do{}while(0)
#endif


#if MPLAYER_DEBUG_LEVEL >= DEBUG_LEVEL_DBG
#define MPLAYER_PRINTF(fff, aaa...)										\
	do {																\
		printf("%s[%d], %s()::\n" fff, __FILE__, __LINE__, __FUNCTION__, ##aaa); \
	}while(0)
#else
#define MPLAYER_PRINTF(fff, aaa...) do{}while(0)
#endif

#if MPLAYER_DEBUG_LEVEL >= DEBUG_LEVEL_INFO
#define MPLAYER_INFO(fff, aaa...)										\
	do {																\
		printf("%s[%d], %s()::\n" fff, __FILE__, __LINE__, __FUNCTION__, ##aaa); \
	}while(0)
#else
#define MPLAYER_INFO(fff, aaa...) do{}while(0)
#endif


#if MPLAYER_DEBUG_LEVEL >= DEBUG_LEVEL_MSG
#define MPLAYER_MSG(fff, aaa...)										\
	do {																\
		printf("%s[%d], %s()::\n" fff, __FILE__, __LINE__, __FUNCTION__, ##aaa); \
	}while(0)
#else
#define MPLAYER_MSG(fff, aaa...) do{}while(0)
#endif





#if MPLAYER_DEBUG_LEVEL >= DEBUG_LEVEL_WARN
#define MPLAYER_WARN(fff, aaa...)										\
	do {																\
		printf("%s[%d], %s()::\n" fff, __FILE__, __LINE__, __FUNCTION__, ##aaa); \
	}while(0)

#else
#define MPLAYER_WARN(fff, aaa...) do{}while(0)
#endif



#if MPLAYER_DEBUG_LEVEL >= DEBUG_LEVEL_ERR
#define MPLAYER_ERR(fff, aaa...)										\
	do {																\
		printf("%s[%d], %s()::\n" fff, __FILE__, __LINE__, __FUNCTION__, ##aaa); \
	}while(0)

#else
#define MPLAYER_ERR(fff, aaa...) do{}while(0)
#endif



#if MPLAYER_DEBUG_LEVEL >= DEBUG_LEVEL_ENTER
#define ENTER()															\
	do {																\
		printf("%s[%d], %s():: *** ENTER ***\n", __FILE__, __LINE__, __FUNCTION__); \
	} while (0)

#else
#define ENTER() do{}while(0)
#endif


#if MPLAYER_DEBUG_LEVEL >= DEBUG_LEVEL_LEAVE
#define LEAVE()															\
	do {																\
		printf("%s[%d], %s():: *** LEAVE ***\n", __FILE__, __LINE__, __FUNCTION__); \
	} while (0)

#else
#define LEAVE() do{}while(0)
#endif





#endif /* __MPLAYER_DEBUG_H__ */

