#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

static const char *dbg_name = "sd:/debug.txt";

int logDebug(const char *fmt, ...)
{
	static int first = 0;
	char lbuf[1024];
	va_list args;
	FILE *fp;

	va_start(args, fmt);
	vsnprintf(lbuf,sizeof(lbuf)-1,fmt,args);
	va_end(args);
	lbuf[sizeof(lbuf)-1] = '\0';   /* tappo */
	
	if(first == 0) {
		fp = fopen(dbg_name,"w");
		first++;
	}
	else
		fp = fopen(dbg_name,"a+");
	if(fp != NULL) {
		fwrite(lbuf,strlen(lbuf),1,fp);
		fclose(fp);
		return 0;
	}
	return -1;
}