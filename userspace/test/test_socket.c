#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <signal.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <fcntl.h>
#include <sys/un.h>

char * const myargs[] = {
		"/work/teapot/userspace/test/test_tty",
		NULL
};

int test_socketpair(void)
{
	int pid, sv[2];
	int num = 1;

	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sv)) {
		return -1;
	}

	pid = fork();
	if(!pid){
		close(sv[0]);

		sleep(3);

		num = 2;
		printf("%-5d write[%d]\n", getpid(), num);
		write(sv[1], &num, sizeof(num));

		read(sv[1], &num, sizeof(num));
		printf("%-5d read[%d]\n", getpid(), num);

		sleep(5);

		/* Be sure we don't inherit this after the exec */
		fcntl(sv[1], F_SETFD, FD_CLOEXEC);
		execvp(myargs[0], myargs);

		printf("%-5d exit\n", getpid());
		return 0;
	}

	close(sv[1]);

	printf("wait read....\n");

	read(sv[0], &num, sizeof(num));
	printf("%-5d read[%d]\n", getpid(), num);

	sleep(3);

	num = 3;
	printf("%-5d write[%d]\n", getpid(), num);
	write(sv[0], &num, sizeof(num));

	/* try test read after execv*/
	printf("try test read after execv, n=0 is right\n");
	int n = read(sv[0], &num, sizeof(num));
	printf("n=%d\n", n);

	printf("%-5d wait pid:%d\n", getpid(), pid);

	waitpid(pid, NULL, 0);

	printf("%-5d end main\n", getpid());


	return 0;
}

int test_ifa()
{
	struct ifaddrs *ifaddr, *ifa;
	if(getifaddrs(&ifaddr) == -1){
		printf("getifaddrs error\n");
		return -1;
	}

	printf("%-10s\t%-10s\t%-5s\n", "ifa_name", "ifa_index", "family");
	for(ifa = ifaddr;ifa;ifa = ifa->ifa_next)
	{
		int family = ifa->ifa_addr->sa_family;
		printf("%-10s\t%-10d\t%-5d\n", ifa->ifa_name, if_nametoindex(ifa->ifa_name), ifa->ifa_addr->sa_family);
	}

	freeifaddrs(ifaddr);
	return 0;
}

int test_af_unix(int flag)
{
	int fd;
	struct sockaddr_un addr;

	fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (fd < 0)
		return -1;

	memset(&addr, 0, sizeof(addr));

	addr.sun_family = AF_UNIX;

	if(flag){
		addr.sun_path[0] = 0;
		strcpy(addr.sun_path+1, "teapot.sock.xx");
	}
	else
		strcpy(addr.sun_path, "teapot.sock.xx");

	if(bind(fd, (struct sockaddr *)&addr, sizeof(addr))){
		close(fd);
		return -1;
	}

	if(listen(fd, 100)){
		close(fd);
		return -1;
	}

	sleep(10);

	{
		struct sockaddr_un addrs;
		socklen_t addrlen;

		if (!getsockname(fd, (struct sockaddr *)&addrs, &addrlen) && addrs.sun_path[0]){
			printf("addrs.sun_path:%s\n", addrs.sun_path);
			unlink(addrs.sun_path);
		}

		close(fd);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	int opt;

	while((opt = getopt(argc, argv, "pau:"))!=-1)
	{
		switch (opt){
		case 'p':
			test_socketpair();
			break;
		case 'a':
			test_ifa();
			break;
		case 'u':
			{
				int flag = atoi(optarg) ? 1 : 0;
				test_af_unix(flag);
				break;
			}
		}
	}

	return 0;
}
