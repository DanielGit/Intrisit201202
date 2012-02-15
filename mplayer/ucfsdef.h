#ifndef __UCFSDEF__
#define __UCFSDEF__

#include <fcntl.h>
#include <jzfs_api.h>

#define FILE   JZFS_FILE
#define fopen  jzfs_Open
#define fclose jzfs_Close
#define fread  jzfs_Read
#define fwrite jzfs_Write
#define fseek  jzfs_Seek
#define ftell  jzfs_Tell
#define ferror  jzfs_Error

#undef fgets
//char *fgets(char *line,int size,FILE *fp);
#undef fprintf
#define fprintf(x,y,c...) ({printf("%s %d",__FILE__,__LINE__); printf(y,##c);})
#undef stderr
#define stderr 1
#undef stdout
#define stdout 2
#undef vfprintf
#define vfprintf fprintf
//int fprintf(FILE * stream, const char * format,...);
#define EOF 0 
#define SEEK_CUR 1
#define SEEK_END 2
#define SEEK_SET 0
#undef mkdir
#define mkdir(x,y) jzfs_Mkdir(x)
#undef rmdir
#define rmdir jzfs_RmDir 

#endif //__UCFSDEF__
