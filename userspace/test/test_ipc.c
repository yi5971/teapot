#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define IPC_ADDR "/tmp/ipctest"

int ipc_send_message(char *addr, char *data, int length)
{
	int sockfd;
	struct sockaddr_un server;
	int result;

	sockfd = socket(AF_UNIX,SOCK_STREAM,0);
	if(sockfd < 0){
		printf("create AF_UNIX socket error %s %d : %s \n",__func__,__LINE__,strerror(errno));
		return -1;
	}

	memset(&server,0,sizeof(struct sockaddr_un));
	server.sun_family = AF_UNIX;
	strncpy(server.sun_path,addr,sizeof(server.sun_path)-1);

	result = connect(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr_un));
	if(result == -1){
		printf("connect AF_UNIX error %s %d : %s \n",__func__,__LINE__,strerror(errno));
		goto out;
	}

	result = write(sockfd, data, length);
	printf("result:%d, send data:%s, length:%d\n", result, data, length);

out:
	close(sockfd);
	return 0;
}

int ipc_start_listening(char *addr)
{
	struct sockaddr_un server, client;
	int ret;
	int listensock, connsock;

	unlink(addr);
	listensock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(listensock == -1){
		printf("error %s %d : %s \n",__func__,__LINE__,strerror(errno));
		return -1;
	}

	memset(&server,0,sizeof(struct sockaddr_un));
	server.sun_family = AF_UNIX;
	strncpy(server.sun_path,addr,sizeof(server.sun_path)-1);

	ret = bind(listensock,(struct sockaddr *)&server,sizeof(struct sockaddr_un));
	if(ret == -1){
		printf("error %s %d : %s \n",__func__,__LINE__,strerror(errno));
		unlink(addr);
		close(listensock);
		return ret;
	}

	ret = listen(listensock, 10);
	if(ret < 0){
		printf("server : listen error\n");
		unlink(addr);
		close(listensock);
		return -1;
	}

	memset(&client,0,sizeof(struct sockaddr_un));
	int clen = sizeof(struct sockaddr_un);
	connsock = accept(listensock, (struct sockaddr *)&client,&clen);
	if(connsock < 0)
	{
		printf("server : accept error:%s\n", strerror(errno));
		unlink(addr);
		close(listensock);
		return -1;
	}

	char buf[64];
	ret = read(connsock, buf, 64);
	if(ret < 0){
		printf("server : recv error:%s\n", strerror(errno));
		close(connsock);
		unlink(addr);
		close(listensock);
		return -1;
	}
	close(connsock);

	printf("rcv buf:%s, ret:%d\n", buf, ret);

	unlink(addr);
	close(listensock);

	return 0;
}

int ipc_stop_listening(char *addr)
{
	return 0;
}

int main(int argc, char *argv[])
{
	if(*argv[1] == 's')
		ipc_start_listening(IPC_ADDR);
	if(*argv[1] == 'c')
		ipc_send_message(IPC_ADDR, argv[2], strlen(argv[2]));

	return 0;
}
