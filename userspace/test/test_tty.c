#include <stdio.h>
#include <unistd.h>
#include <pty.h>

int main(int argv, char *argc[])
{
	int opt;

	while((opt = getopt(argv, argc, "s")) != -1)
	{
		switch(opt){
		case 's':
			break;
		}
	}

	char tty[1024];
	if (ttyname_r(0, tty, sizeof(tty)))
		tty[0] = '\0';

	printf("tty:%s\n", tty);

	int master, slave;
	char name[1024];
	if(!openpty(&master, &slave, name, NULL, NULL))
	{
		printf("%d, %d, %s\n", master, slave, name);

		sleep(10);

		close(master);
		close(slave);
	}


	return 0;
}
