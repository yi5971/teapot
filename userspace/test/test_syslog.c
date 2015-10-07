#include <stdio.h>
#include <unistd.h>
#include <syslog.h>

int main(int agrv, char *argc[])
{
	int i=0;
	while(1){
//	syslog(LOG_USER | LOG_DEBUG, "func name [%s], %d\n", argc[0], i);
	printf("%d\n", i++);
	sleep(5);
        }	
	return 0;
}

/*
 * ÅäÖÃÎÄ¼şÔÚ/etc/rsyslog.d/50-default.conf
 * sudo restart rsyslog
 * */
