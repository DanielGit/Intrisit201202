//[PROPERTY]===========================================[PROPERTY]
//            *****   电子词典平台  *****
//        --------------------------------------
//       	         LZW解压部分函数
//        --------------------------------------
//                 版权: 新诺亚舟科技
//             ---------------------------
//                  版   本   历   史
//        --------------------------------------
//  版本    日前		说明
//  V0.1    2007-11      Init.             Hisway.Gao
//[PROPERTY]===========================================[PROPERTY]

#ifndef _LZW_H
#define _LZW_H


#define INIT_BITS		9
#define MAX_BITS		14           /* Do not exceed 14 with this program */
#define HASHING_SHIFT	MAX_BITS - 8

#if MAX_BITS == 14            /* Set the table size. Must be a prime    */
#define TABLE_SIZE		18041      /* number somewhat larger than 2^MAX_BITS.*/
#elif MAX_BITS == 13
#define TABLE_SIZE		9029
#else
#define TABLE_SIZE		5021
#endif

#define CLEAR_TABLE		256    /* Code to flush the string table */
#define TERMINATOR		257    /* To mark EOF Condition, instead of MAX_VALUE */
#define FIRST_CODE		258    /* First available code for code_value table */
#define CHECK_TIME		100    /* Check comp ratio every CHECK_TIME chars input */

#define MAXVAL(n)		(( 1 <<( n )) -1)   /* max_value formula macro */


typedef struct _LZW_RWP
{
	unsigned char *base;
	int offset;
	int size;
}LZW_RWP;

typedef struct _LZW_OBJ
{
	int max_code;									/* old MAX_CODE */
	int num_bits;									/* Starting with 9 bit codes */
	unsigned int bytes_in;
	unsigned int bytes_out;							/* Used to monitor compression ratio */
	unsigned int checkpoint;						/* For compression ratio monitoring */
	int input_bit_count;
	unsigned int input_bit_buffer;
	LZW_RWP in;
	LZW_RWP out;
	int code_value[TABLE_SIZE];						/* This is the code value array */
	unsigned int prefix_code[TABLE_SIZE];			/* This array holds the prefix codes */
	unsigned char append_character[TABLE_SIZE];		/* This array holds the appended chars */
	unsigned char decode_stack[0x20000];			/* This array holds the decoded string */
}LZW_OBJ;

void kLzwEncode(LZW_OBJ *obj);
void kLzwDecode(LZW_OBJ *obj);

#endif // _LZW_H
