#include <stdio.h>
#include <unistd.h>
#include <pty.h>

#define MAXPATHLEN 1024

struct pty_info {
	char name[MAXPATHLEN];
	int master;
	int slave;
	int busy;
} pty_info [2];

int tty_func(void)
{
	int i;
	struct pty_info *pty = pty_info;

	for(i=0;i<2;i++,pty++){
		if(!openpty(&pty->master, &pty->slave, pty->name, NULL, NULL))
		{
			printf("%d, %d, %s\n", pty->master, pty->slave, pty->name);
			pty->busy = 0;
		}
	}

	sleep(1);

	pty = pty_info;
	for(i=0;i<2;i++,pty++){
		close(pty->master);
		close(pty->slave);
	}

	return 0;
}

int main(int argv, char *argc[])
{
	int opt;

	while((opt = getopt(argv, argc, "s")) != -1)
	{
		switch(opt){
		case 's':
			tty_func();
			break;
		}
	}

	char tty[1024];
	if (ttyname_r(0, tty, sizeof(tty)))
		tty[0] = '\0';

	printf("tty:%s\n", tty);




	return 0;
}
