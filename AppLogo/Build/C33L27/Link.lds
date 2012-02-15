OUTPUT_FORMAT("elf32-c33", "elf32-c33", "elf32-c33")
OUTPUT_ARCH(c33)
SEARCH_DIR(.);
SECTIONS
{
	__dp = 0;
	. = 0;

	/*********************************
	 其他程序代码部分
	**********************************/
	.text : 
	{
		__START_text = . ; 
		*(.text) ;
		__END_text = . ; 
	}

	/*********************************
	 ROM数据部分
	**********************************/
	.rodata :
	{
		__START_rodata = . ; 
		*(.rodata) ;
		__END_rodata = . ; 
	}

	.data __END_rodata :
	{
		__START_data = . ; 
		*(.data) ;
		__END_data = . ; 
	}

	.bss : 
	{
		_start_bss = . ; 
		*(.bss) ;
		_end_bss = . ; 
	}
}

