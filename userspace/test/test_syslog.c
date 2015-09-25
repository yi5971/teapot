#include <stdio.h>
#include <unistd.h>
#include <syslog.h>

int main(int agrv, char *argc[])
{
	syslog(LOG_USER | LOG_DEBUG, "func name [%s]\n", argc[0]);
	return 0;
}

/*
 * ÅäÖÃÎÄ¼şÔÚ/etc/rsyslog.d/50-default.conf
 * sudo restart rsyslog
 * */
