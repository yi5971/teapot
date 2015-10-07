/*
 * mutex.c
 *
 *  Created on: 2015Äê4ÔÂ12ÈÕ
 *      Author: Administrator
 */
#include <stdio.h>
#include<sys/file.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <unistd.h>

#define MEM_SIZE 4096

int main(int argc, char *argv[])
{
	if(argc !=2 ){
		printf("	Usage: %s directory\n", argv[0]);
		exit(-1);
	}

	char *old_path = calloc(MEM_SIZE, 1);
	if(!old_path)
		goto out1;

	char *new_path = calloc(MEM_SIZE, 1);
	if(!new_path)
		goto out2;

	strcpy(old_path, argv[1]);
	printf("   ___________ old_path : %s _________\n", old_path);

	char *slash = NULL, *tmp_slash = NULL, *dir_start = old_path+1;

	while((slash = strchr(dir_start, '/')) != NULL && *dir_start != '\0')
	{
		if(*(slash+1) == '/'){
			printf(" directory formt error! \n");
			goto out3;
		}

		*slash = '\0';
		int mid_dir_len = strlen(dir_start);

		if(mid_dir_len == 1 && *dir_start == '.'){
			dir_start = slash+1;
		}
		else if(mid_dir_len == 2 && !strncmp(dir_start, "..", 2)){
			if(*new_path != '\0' && *(new_path+1) != '\0'){
				tmp_slash = strrchr(new_path, '/');
				if(new_path != tmp_slash)
					*tmp_slash = '\0';
				else
					*(tmp_slash+1) = '\0';
			}
			dir_start = slash+1;
		}
		else{
			printf("mid dir :%s\n", dir_start);
			/* new path is '/' */
			if(strnlen(new_path, 2) != 1)
				strncat(new_path, "/", 1);
			strcat(new_path, dir_start);
			dir_start = slash+1;
		}
	}

	if(*dir_start != '\0' && *dir_start != '.' && !slash){
		if(strnlen(new_path, 2) != 1)
			strncat(new_path, "/", 1);
		strcat(new_path, dir_start);
	}

	if(*new_path == '\0'){
		strncat(new_path, "/", 1);
	}
	printf("   ___________ new_path : %s _________\n", new_path);

out3:
	free(new_path);
out2:
	free(old_path);
out1:
	return 0;
}
