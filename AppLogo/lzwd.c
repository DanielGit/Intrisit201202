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

#include <libc/stdio.h>
#include <libc/stdlib.h>
#include <libc/string.h>
#include "lzw.h"

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
////////////////////////////////////////////////////
static int _LzwGetChar(LZW_RWP *rwp)
{
	if(rwp->offset < rwp->size)
		return rwp->base[rwp->offset++];
	return EOF;
}

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
////////////////////////////////////////////////////
static void _LzwPutChar(int c, LZW_RWP *rwp)
{
	rwp->base[rwp->offset++] = (unsigned char)(c & 0xff);
	rwp->size++;
}

////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
////////////////////////////////////////////////////
static unsigned int _LzwInputCode(LZW_OBJ *obj)
{
	unsigned int return_value;
	
	while (obj->input_bit_count <= 24 ) 
	{
		obj->input_bit_buffer |= (unsigned long) _LzwGetChar(&obj->in) << (24 - obj->input_bit_count);
		obj->input_bit_count += 8;
	}
	return_value=obj->input_bit_buffer >> (32-obj->num_bits);
	obj->input_bit_buffer <<= obj->num_bits;
	obj->input_bit_count -= obj->num_bits;
	return(return_value);
}


////////////////////////////////////////////////////
// 功能: Decode a string from the string table, storing it in a buffer.
// The buffer can then be output in reverse order by the expansion
// program.
// 输入: 
// 输出:
// 返回: 
////////////////////////////////////////////////////
static unsigned char *_LzwDecodeString(LZW_OBJ *obj, unsigned char *buffer, unsigned int code)
{
	int i=0;
	
	while(code > 255 ) 
	{
		*buffer++ = obj->append_character[code];
		code=obj->prefix_code[code];
		if (i++ >= 4000 ) 
		{
			return NULL;
		}
	}
	*buffer=code;
	return buffer;
}


////////////////////////////////////////////////////
// 功能: 
// 输入: 
// 输出:
// 返回: 
////////////////////////////////////////////////////
void LzwDecode(LZW_OBJ *obj)
{
	int new_code;
	unsigned char *string;
	unsigned int old_code=0;
	int character=0;
	int	counter=0;
	int	clear_flag=1;          /* Need to clear the code value array */
	int next_code=FIRST_CODE;
	
	obj->num_bits=INIT_BITS;							/* Starting with 9 bit codes */
	obj->bytes_in=0;
	obj->bytes_out=0;						/* Used to monitor compression ratio */
	obj->checkpoint=CHECK_TIME;				/* For compression ratio monitoring */
	obj->max_code = MAXVAL(obj->num_bits);		/* Initialize max_value & max_code */
	obj->input_bit_count = 0;
	obj->input_bit_buffer = 0;
	while((new_code=_LzwInputCode(obj)) != TERMINATOR) 
	{
		if (clear_flag) 
		{       /* Initialize or Re-Initialize */
			clear_flag=0;
			old_code=new_code;   /* The next three lines have been moved */
			character=old_code;  /* from the original */
			_LzwPutChar(old_code, &obj->out);
			continue;
		}
		if (new_code == CLEAR_TABLE) 
		{     /* Clear string table */
			clear_flag=1;
			obj->num_bits=INIT_BITS;
			next_code=FIRST_CODE;
			obj->max_code = MAXVAL(obj->num_bits);
			continue;
		}
		if (++counter == 1000) 
		{           /* Pacifier */
			counter=0;
		}
		if (new_code >= next_code) 
		{       /* Check for string+char+string */
			*obj->decode_stack=character;
			string = _LzwDecodeString(obj, obj->decode_stack+1,old_code);
		}
		else
			string = _LzwDecodeString(obj, obj->decode_stack,new_code);
		if(string == NULL)
			return;
		
		character = *string;              /* Output decoded string in reverse */
		while (string >= obj->decode_stack)
			_LzwPutChar(*string--, &obj->out);
		
		if (next_code <= obj->max_code)
		{      /* Add to string table if not full */
			obj->prefix_code[next_code]=old_code;
			obj->append_character[next_code++]=character;
			if (next_code == obj->max_code && obj->num_bits < MAX_BITS) 
			{
				obj->max_code = MAXVAL(++obj->num_bits);
			}
		}
		old_code=new_code;
	}
}
