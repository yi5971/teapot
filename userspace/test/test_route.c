#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sched.h>
#include <signal.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if_link.h>

#define TYPE_OP_ADD	1
#define TYPE_OP_DEL	2
#define TYPE_OP_GET	3

#define TYPE_CONTEXT_LINK	1
#define TYPE_CONTEXT_ADDR	2
#define TYPE_CONTEXT_ROUTE	3
/* man 7 rtnetlink */

struct link_req {
	struct nlmsghdr nh;
	struct ifinfomsg ifa;
	char rtattr[1024];
};


static void Usage(char *proc){
	printf("%s -[a|d|g] -[L|A|R]\n", proc);
}

#define NLMSG_TAIL(nmsg) \
        ((struct rtattr *) (((void *) (nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))

static int rta_put(struct link_req *req, int attr,
		   const void *data, size_t len)
{
	struct rtattr *rta;
	size_t rtalen = RTA_LENGTH(len);

	rta = NLMSG_TAIL(&req->nh);
	rta->rta_type = attr;
	rta->rta_len = rtalen;
	memcpy(RTA_DATA(rta), data, len);
	req->nh.nlmsg_len = NLMSG_ALIGN(req->nh.nlmsg_len) + RTA_ALIGN(rtalen);

	return 0;
}

struct rtattr *rta_begin_nested(struct link_req *req, int attr)
{
	struct rtattr *rtattr = NLMSG_TAIL(&req->nh);

	if (rta_put(req, attr, NULL, 0))
		return NULL;

	return rtattr;
}

void rta_end_nested(struct link_req *req, struct rtattr *attr)
{
	attr->rta_len = (void *)NLMSG_TAIL(&req->nh) - (void *)attr;
}

int add_link()
{
	struct rtattr *nest;
	struct link_req req, ans;
	int rtnetlink_sk, ifindex;
	int ret;

	rtnetlink_sk = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);

	memset(&req, 0, sizeof(req));
	req.nh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
	req.nh.nlmsg_flags = NLM_F_REQUEST;
	req.nh.nlmsg_type = RTM_NEWLINK;
	req.ifa.ifi_family = AF_UNSPEC;

	printf("nh:%ld, ifa:%ld, nlmsg_len:%d\n", sizeof(req.nh), sizeof(req.ifa), req.nh.nlmsg_len);

	nest = rta_begin_nested(&req, IFLA_LINKINFO);
	rta_put(&req, IFLA_INFO_KIND, "macvlan", strlen("macvlan")+1);
	rta_end_nested(&req, nest);

	printf("rta_len:%d, nlmsg_len:%d\n", nest->rta_len, req.nh.nlmsg_len);

	ifindex = if_nametoindex("eth0");
	rta_put(&req, IFLA_LINK, &ifindex, sizeof(int));

	printf("nlmsg_len:%d\n", req.nh.nlmsg_len);

	rta_put(&req, IFLA_IFNAME, "macvlan001", strlen("macvlan001")+1);

	printf("nlmsg_len:%d\n", req.nh.nlmsg_len);

	printf("send request for add macvlan001\n");
	ret = send(rtnetlink_sk, &req, req.nh.nlmsg_len, 0);
	printf("send ret:%d\n", ret);

	ret = recv(rtnetlink_sk, &ans, NLMSG_LENGTH(sizeof(struct ifinfomsg) + 1024), 0);
	printf("recv ret:%d, nlmsg_type:%d\n", ret, ans.nh.nlmsg_type);

	if (ans.nh.nlmsg_type == NLMSG_ERROR) {
		struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(&ans);
		printf("error:%d\n", err->error);
		return err->error;
	}

	return 0;
}

int del_link(){
	return 0;
}

int main(int argc, char *argv[])
{
	int opt, op = 0, context = 0;

	while((opt = getopt(argc, argv, "adgLAR"))!=-1)
	{
		switch (opt){
		case 'a':
			op = TYPE_OP_ADD;
			break;
		case 'd':
			op = TYPE_OP_DEL;
			break;
		case 'g':
			op = TYPE_OP_GET;
			break;
		case 'L':
			context = TYPE_CONTEXT_LINK;
			break;
		case 'A':
			context = TYPE_CONTEXT_ADDR;
			break;
		case 'R':
			context = TYPE_CONTEXT_ROUTE;
			break;
		}
	}

	if(!op || !context){
		Usage(argv[0]);
		return -1;
	}

	if(context == TYPE_CONTEXT_LINK){
		switch (op){
		case TYPE_OP_ADD:
			add_link();
			break;
		case TYPE_OP_DEL:
			del_link();
			break;
		}
	}

	return 0;
}
