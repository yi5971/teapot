#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <pwd.h>
#include <errno.h>

#define MAXLINE 1024

#define PROGNAME_WIDTH 20

#define PROGNAME_WIDTHs PROGNAME_WIDTH1(PROGNAME_WIDTH)
#define PROGNAME_WIDTH1(s) PROGNAME_WIDTH2(s)
#define PROGNAME_WIDTH2(s) #s

#define PRG_HASH_SIZE 211

static struct prg_node {
    struct prg_node *next;
    int inode;
    char name[PROGNAME_WIDTH];
} *prg_hash[PRG_HASH_SIZE];

static char prg_cache_loaded = 0;

#define PRG_HASHIT(x) ((x) % PRG_HASH_SIZE)

#define PROGNAME_BANNER "PID/Program name"

#define print_progname_banner() do { if (flag_prg) printf("%-" PROGNAME_WIDTHs "s"," " PROGNAME_BANNER); } while (0)

#define PRG_LOCAL_ADDRESS "local_address"
#define PRG_INODE	 "inode"
#define PRG_SOCKET_PFX    "socket:["
#define PRG_SOCKET_PFXl (strlen(PRG_SOCKET_PFX))
#define PRG_SOCKET_PFX2   "[0000]:"
#define PRG_SOCKET_PFX2l  (strlen(PRG_SOCKET_PFX2))


#ifndef LINE_MAX
#define LINE_MAX 4096
#endif

#define PATH_PROC	   "/proc"
#define PATH_FD_SUFF	"fd"
#define PATH_FD_SUFFl       strlen(PATH_FD_SUFF)
#define PATH_PROC_X_FD      PATH_PROC "/%s/" PATH_FD_SUFF
#define PATH_CMDLINE	"cmdline"
#define PATH_CMDLINEl       strlen(PATH_CMDLINE)
/* NOT working as of glibc-2.0.7: */
#define PORT_MAX 65535

static void prg_cache_add(int inode, char *name)
{
    unsigned hi = PRG_HASHIT(inode);
    struct prg_node **pnp,*pn;

    prg_cache_loaded=2;
    for (pnp=prg_hash+hi;(pn=*pnp);pnp=&pn->next) {
	if (pn->inode==inode) {
	    /* Some warning should be appropriate here
	       as we got multiple processes for one i-node */
	    return;
	}
    }
    if (!(*pnp=malloc(sizeof(**pnp)))) 
	return;
    pn=*pnp;
    pn->next=NULL;
    pn->inode=inode;
    if (strlen(name)>sizeof(pn->name)-1) 
	name[sizeof(pn->name)-1]='\0';
    strcpy(pn->name,name);
}

static const char *prg_cache_get(int inode)
{
    unsigned hi=PRG_HASHIT(inode);
    struct prg_node *pn;

    for (pn=prg_hash[hi];pn;pn=pn->next)
	if (pn->inode==inode) return(pn->name);
    return("-");
}

static void prg_cache_clear(void)
{
    struct prg_node **pnp,*pn;

    if (prg_cache_loaded == 2)
	for (pnp=prg_hash;pnp<prg_hash+PRG_HASH_SIZE;pnp++)
	    while ((pn=*pnp)) {
		*pnp=pn->next;
		free(pn);
	    }
    prg_cache_loaded=0;
}

static void extract_type_1_socket_inode(const char lname[], long * inode_p) {

    /* If lname is of the form "socket:[12345]", extract the "12345"
       as *inode_p.  Otherwise, return -1 as *inode_p.
       */

    if (strlen(lname) < PRG_SOCKET_PFXl+3) *inode_p = -1;
    else if (memcmp(lname, PRG_SOCKET_PFX, PRG_SOCKET_PFXl)) *inode_p = -1;
    else if (lname[strlen(lname)-1] != ']') *inode_p = -1;
    else {
        char inode_str[strlen(lname + 1)];  /* e.g. "12345" */
        const int inode_str_len = strlen(lname) - PRG_SOCKET_PFXl - 1;
        char *serr;

        strncpy(inode_str, lname+PRG_SOCKET_PFXl, inode_str_len);
        inode_str[inode_str_len] = '\0';
        *inode_p = strtol(inode_str,&serr,0);
        if (!serr || *serr || *inode_p < 0 || *inode_p > PORT_MAX) 
            *inode_p = -1;
    }
}



static void extract_type_2_socket_inode(const char lname[], long * inode_p) {

    /* If lname is of the form "[0000]:12345", extract the "12345"
       as *inode_p.  Otherwise, return -1 as *inode_p.
       */

    if (strlen(lname) < PRG_SOCKET_PFX2l+1) *inode_p = -1;
    else if (memcmp(lname, PRG_SOCKET_PFX2, PRG_SOCKET_PFX2l)) *inode_p = -1;
    else {
        char *serr;

        *inode_p=strtol(lname + PRG_SOCKET_PFX2l,&serr,0);
        if (!serr || *serr || *inode_p < 0 || *inode_p > PORT_MAX) 
            *inode_p = -1;
    }
}

static void prg_cache_load(void)
{
    char line[LINE_MAX],eacces=0;
    int procfdlen,fd,cmdllen,lnamelen;
    char lname[30],cmdlbuf[512],finbuf[PROGNAME_WIDTH];
    long inode;
    const char *cs,*cmdlp;
    DIR *dirproc=NULL,*dirfd=NULL;
    struct dirent *direproc,*direfd;

    if (prg_cache_loaded) return;
    prg_cache_loaded=1;
    cmdlbuf[sizeof(cmdlbuf)-1]='\0';
    if (!(dirproc=opendir(PATH_PROC))) goto fail;
    while (errno=0,direproc=readdir(dirproc)) {
	for (cs=direproc->d_name;*cs;cs++)
	    if (!isdigit(*cs)) 
		break;
	if (*cs) 
	    continue;
	procfdlen=snprintf(line,sizeof(line),PATH_PROC_X_FD,direproc->d_name);
	if (procfdlen<=0 || procfdlen>=sizeof(line)-5) 
	    continue;
	errno=0;
	dirfd=opendir(line);
	if (! dirfd) {
	    if (errno==EACCES) 
		eacces=1;
	    continue;
	}
	line[procfdlen] = '/';
	cmdlp = NULL;
	while ((direfd = readdir(dirfd))) {
	    if (procfdlen+1+strlen(direfd->d_name)+1>sizeof(line)) 
		continue;
	    memcpy(line + procfdlen - PATH_FD_SUFFl, PATH_FD_SUFF "/",
		   PATH_FD_SUFFl+1);
	    strcpy(line + procfdlen + 1, direfd->d_name);
	    lnamelen=readlink(line,lname,sizeof(lname)-1);
            lname[lnamelen] = '\0';  /*make it a null-terminated string*/

            extract_type_1_socket_inode(lname, &inode);

            if (inode < 0) extract_type_2_socket_inode(lname, &inode);

            if (inode < 0) continue;

	    if (!cmdlp) {
		if (procfdlen - PATH_FD_SUFFl + PATH_CMDLINEl >= 
		    sizeof(line) - 5) 
		    continue;
		strcpy(line + procfdlen-PATH_FD_SUFFl, PATH_CMDLINE);
		fd = open(line, O_RDONLY);
		if (fd < 0) 
		    continue;
		cmdllen = read(fd, cmdlbuf, sizeof(cmdlbuf) - 1);
		if (close(fd)) 
		    continue;
		if (cmdllen == -1) 
		    continue;
		if (cmdllen < sizeof(cmdlbuf) - 1) 
		    cmdlbuf[cmdllen]='\0';
		if ((cmdlp = strrchr(cmdlbuf, '/'))) 
		    cmdlp++;
		else 
		    cmdlp = cmdlbuf;
	    }

	    snprintf(finbuf, sizeof(finbuf), "%s/%s", direproc->d_name, cmdlp);
	    prg_cache_add(inode, finbuf);
	}
	closedir(dirfd); 
	dirfd = NULL;
    }
    if (dirproc) 
	closedir(dirproc);
    if (dirfd) 
	closedir(dirfd);
    if (!eacces) 
	return;
    if (prg_cache_loaded == 1) {
    fail:
	fprintf(stderr,"(No info could be read for \"-p\": geteuid()=%d but you should be root.)\n",
		geteuid());
    }
    else
	fprintf(stderr, "(Not all processes could be identified, non-owned process info\n"
			 " will not be shown, you would have to be root to see it all.)\n");
}


int main(int argc,char ** argv)
{
	int i;
	char *cmdline;
	prg_cache_load();
	for(i=1;i<PORT_MAX;i++)
	{
		cmdline = prg_cache_get(i);
		if(*cmdline != '-')
			printf("port:[%d], cmdline:[%s]\n", i, cmdline);
	}
	printf("over======PORT_MAX:%d=\n", PORT_MAX);
	prg_cache_clear();
	return 0;
}

