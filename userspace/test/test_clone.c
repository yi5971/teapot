#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <signal.h>
#include <sys/mount.h>

#define STACK_SIZE 1024*1024
static char stacks[STACK_SIZE];

char * const clone_args[] = {
		"/bin/bash",
		NULL,
};

int clone_fun(void *arg){
	printf("%d start clone func\n", getpid());

	sethostname("liujing's ubuntu", strlen("liujing's ubuntu"));
	//chroot("/work");
	int ret = mount("/work/teapot", "work/1/", 0x410284, MS_MGC_VAL|MS_BIND, NULL);
	printf("ret:%d\n", ret);
	execv(clone_args[0], clone_args);


	printf("%d end clone func\n", getpid());

	return 0;
}

int main(int agrv, char *argc[])
{
	int pid;
	int clone_flags = CLONE_NEWPID|CLONE_NEWIPC|CLONE_NEWNS|CLONE_NEWUTS|CLONE_NEWNET|SIGCHLD;

	pid = clone(clone_fun, stacks+STACK_SIZE, clone_flags, NULL);

	printf("wait pid:%d\n", pid);


	waitpid(pid, NULL, 0);


	printf("end main\n");


	return 0;
}
