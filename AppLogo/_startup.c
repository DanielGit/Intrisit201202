
#include <noahos.h>

extern void __libc_init(void);
extern int main(int argc, char **argv);

void noahos_main(int base, void (*entry)(void), int argc, char **argv)
{
	__libc_init();
	main(argc, argv);
	for(;;)
	{
		TaskTerminate(TaskSelf());
	}
}

