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
#include "../utils/list.h"

#define MAXLINE 1024

int isdir(char *path)
{
	struct stat buf;
	if(lstat(path,&buf)<0)
	{
// 	  printf("lstat err for %s \n",path);		
		return 0;
	}

	if(S_ISDIR(buf.st_mode)||S_ISLNK(buf.st_mode))
	{
		return 1;
	}
	return 0;
}

char * getfile(char *path)
{
	static char filename[MAXLINE];
	char *tmp1,*tmp2,filepath[MAXLINE];

	if(path==NULL)
		return NULL;
	memset(filepath,0,sizeof(filepath));
	strcpy(filepath,path);
	tmp2=tmp1=strtok(filepath,"/");
	while((tmp1=strtok(NULL,"/")))
	{
		tmp2=tmp1;
	}
	memset(filename,0,sizeof(filename));
	if(tmp2)
	{
		strcpy(filename,tmp2);
		tmp1=filename;
		while(*tmp1&&*tmp1++!=' ');
			*(tmp1)='\0';
	}
	else
		return NULL;
	return filename;
}

void readstatus(char *filename)
{
	FILE *file;
	static char buf[MAXLINE];

	file=fopen(filename,"r");
	if(!file)
		return;
	while(fgets(buf,sizeof(buf),file))
	{
		if(strncmp(buf,"State",5)==0)
		{
			printf("%s\n",buf);
			break;
		}
	}
	fclose(file);
}

struct mem_table_struct {
  const char *name;     /* memory type name */
  unsigned long *value; /* slot in return struct */
};

static float g_MemUsedRate;
static unsigned long g_MemTotal;

static float get_mem_rate(void)
{
	return g_MemUsedRate;
}

static float get_mem_total(void)
{
	return g_MemTotal;
}

void sys_mem_stat(void)
{
	FILE *fd;
	fd = fopen ("/proc/meminfo", "r");
	char line_buff[256] = {0};
	unsigned long mem_total = -1, mem_free = -1, mem_buffers = -1, mem_cached = -1;
	const struct mem_table_struct mem_table[] = {
			{"MemTotal:", &mem_total},
			{"MemFree:", &mem_free},
		  	{"Buffers:", &mem_buffers},
  			{"Cached:", &mem_cached},
		};
	int mem_table_count = sizeof(mem_table) / sizeof(mem_table[0]);
	int count = 0, value, i;
	char name[32];
	float mem_rate;

	while(!feof(fd) && count != mem_table_count)
	{
		fgets (line_buff, sizeof(line_buff), fd);
		sscanf (line_buff, "%s%d", name, &value);
		
		for(i = 0;i < mem_table_count && count != mem_table_count;i++){
			if(*mem_table[i].value != -1 || strcmp(name, mem_table[i].name))
				continue;
			*mem_table[i].value = value;
			count++;
			//printf ("====[%s][%d]====\n", name,value);
		}
	}
	fclose(fd);

	mem_rate = (float)(mem_total - mem_free - mem_buffers - mem_cached) / mem_total * 100;

//	printf("memory rate : %f\n", mem_rate);

	g_MemUsedRate = mem_rate;
	g_MemTotal = mem_total;
	
	return;
}

int get_process_mem_rate(char *ProcPath, unsigned long MemTotal, float *ProcMemUsedRate, unsigned long *ProcMemTotal)
{
	FILE *fd;
	char filename[80];
	char line_buff[256] = {0};
	unsigned long mem_rss = -1;
	const struct mem_table_struct mem_table[] = {
			{"VmRSS:", &mem_rss},
		};
	int mem_table_count = sizeof(mem_table) / sizeof(mem_table[0]);
	int count = 0, value, i;
	char name[32];
	float mem_rate;

	snprintf(filename, 80, "%s/status", ProcPath);
	fd = fopen(filename, "r");
	while(!feof(fd) && count != mem_table_count)
	{
		fgets (line_buff, sizeof(line_buff), fd);
		sscanf (line_buff, "%s%d", name, &value);
		
		for(i = 0;i < mem_table_count && count != mem_table_count;i++){
			if(*mem_table[i].value != -1 || strcmp(name, mem_table[i].name))
				continue;
			*mem_table[i].value = value;
			count++;
			//printf ("====[%s][%d]====\n", name,value);
		}
	}
	fclose(fd);

	if(mem_rss == -1)	
		mem_rss = 0;
	
	mem_rate = (float)mem_rss / MemTotal * 100;

//	printf("process[%s] memory rate : %.2f\n", filename, mem_rate);

	*ProcMemUsedRate = mem_rate;
	*ProcMemTotal = mem_rss;
	
	return mem_rate;
}

int get_cpu_rate(void)
{
	FILE *fp;
	fp = fopen("/proc/stat","r");
	char line_buff[256] = {0};
	char cpu[5];
	long int user,nice,sys,idle,iowait,irq,softirq;
	long int all1,all2,idle1,idle2;
	float usage;

	fgets(line_buff,sizeof(line_buff),fp);
	sscanf(line_buff,"%s%ld%ld%ld%ld%ld%ld%ld",cpu,&user,&nice,&sys,&idle,&iowait,&irq,&softirq);

	all1 = user+nice+sys+idle+iowait+irq+softirq;
	idle1 = idle;
	rewind(fp);
	
	sleep(1);
	memset(line_buff,0,sizeof(line_buff));
	cpu[0] = '\0';
	user=nice=sys=idle=iowait=irq=softirq=0;
	
	fgets(line_buff,sizeof(line_buff),fp);
	sscanf(line_buff,"%s%ld%ld%ld%ld%ld%ld%ld",cpu,&user,&nice,&sys,&idle,&iowait,&irq,&softirq);

	all2 = user+nice+sys+idle+iowait+irq+softirq;
	idle2 = idle;
	
	usage = (float)(all2-all1-(idle2-idle1)) / (all2-all1)*100;

	printf("all=%ld\n",all2-all1);
	printf("ilde=%ld\n",all2-all1-(idle2-idle1));
	printf("cpu use = %.2f\n",usage);
	printf("=======================\n");
	fclose(fp);
	return 0;
	
}

#if 0
int main(int argc,char ** argv)
{
	float MemUsedRate;
	unsigned long MemTotal;
	
//	get_mem_rate(&MemUsedRate, &MemTotal);
	get_cpu_rate();
	
	return 0;
}
#endif

static int file2str(const char *directory, const char *what, char *ret, int cap) {
    static char filename[80];
    int fd, num_read;

    sprintf(filename, "%s/%s", directory, what);
    fd = open(filename, O_RDONLY, 0);
    if(fd==-1) return -1;
    num_read = read(fd, ret, cap - 1);
    close(fd);
    if(num_read<=0) return -1;
    ret[num_read] = '\0';
    return num_read;
}

struct proc_node {
	struct list_head list;
	int pid;
	char path[128];
	char comm[32];
	char user[64];
	unsigned long mem_used;
	float mem_usage;
};

LIST_HEAD(proc_list);
static int proc_count;


static int proc_node_add(int pid, char *path, char *comm, char *user, unsigned long mem_used, float mem_usage)
{
	struct proc_node *node;
	int slen;

	node = malloc(sizeof(struct proc_node));
	if(!node)
		return -1;
	INIT_LIST_HEAD(&node->list);
	node->pid = pid;
	
	slen = strlen(path) > 128 ? 128-1 : strlen(path);
	strcpy(node->path, path);
	node->path[slen] = '\0';

	slen = strlen(comm) > 32 ? 32-1 : strlen(comm);
	strcpy(node->comm, comm);
	node->comm[slen] = '\0';

	slen = strlen(user) > 64 ? 64-1 : strlen(user);
	strcpy(node->user, user);
	node->user[slen] = '\0';
	
	node->mem_used = mem_used;
	node->mem_usage = mem_usage;

	list_add_tail(&node->list, &proc_list);
	proc_count++;
	return 0;
}

struct proc_node * proc_node_get(struct proc_node *node)
{
	if(!proc_count)
		return NULL;
		
	if(!node)
		return list_entry(proc_list.next, struct proc_node, list);

	if(node->list.next == &proc_list)
		return NULL;

	return list_entry(node->list.next, struct proc_node, list);
}

void proc_node_clear(void)
{
	struct proc_node *node1, *node2;

	list_for_each_entry_safe(node1, node2, &proc_list, list)
	{
		list_del(&node1->list);
		free(node1);
	}
	proc_count = 0;
}

void proc_node_init(void)
{
	char buf1[MAXLINE],buf2[MAXLINE];
	char ProcPath[MAXLINE],*ptr, comm[80];
	DIR *db,*directory;struct dirent *p;
	float MemUsedRate;
	unsigned long MemTotal;

	MemUsedRate = get_mem_rate();
	MemTotal = get_mem_total();

	db=opendir("/proc/");
	if(db==NULL)
		return ;

	while(p=readdir(db))
	{
		char *cs;
		for (cs=p->d_name;*cs;cs++)
			if (!isdigit(*cs)) 
				break;
		if (*cs) 
			continue;

//		printf("cs:%s\n", p->d_name);
		sprintf(ProcPath, "/proc/%s", p->d_name);

		file2str(ProcPath, "comm", comm, sizeof comm);
		ptr = strstr(comm, "\n");
		*ptr = '\0';

		sprintf(buf1,"%s/exe", ProcPath);
		memset(buf2,0,sizeof(buf2));
		if(readlink(buf1,buf2,sizeof(buf2))==-1){
			sprintf(buf2,"[%s]", comm);
		}
		//printf("%s\n", buf2);

		static struct stat sb;
		stat(ProcPath, &sb);
		
		struct passwd *pw;
		pw = getpwuid(sb.st_uid);

		float ProcMemUsedRate;
		unsigned long ProcMemTotal;
		get_process_mem_rate(ProcPath, MemTotal, &ProcMemUsedRate, &ProcMemTotal);

		//printf("pid: %d, path: %s, user: %s  memory usage: %dKB|%.1f%\n", atoi(p->d_name), buf2, pw->pw_name, ProcMemTotal, ProcMemUsedRate);
		proc_node_add(atoi(p->d_name), buf2, comm, pw->pw_name, ProcMemTotal, ProcMemUsedRate);
		
	}

	closedir(db);
}

int main(int argc,char ** argv)
{
	proc_node_init();
	printf("proc_node:%d\n", proc_count);
	struct proc_node *node = NULL;
	while((node = proc_node_get(node)))
	{
		printf("pid: %d, comm: %s, path: %s, user: %s  memory usage: %dKB|%.1f%\n", node->pid, node->comm, node->path, node->user, node->mem_used, node->mem_usage);
	}
	proc_node_clear();
}

