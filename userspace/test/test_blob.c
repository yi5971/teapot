#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

int main(int argc, char *argv[])
{
	sqlite3 *db;
	char *err;
	unsigned char hex[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
	sqlite3_stmt *stmt;

	if(SQLITE_OK != sqlite3_open("./test.db", &db))
	{
		printf("error\n");
		return -1;
	}
	char sqlbuf[1024];
	int len;

	len = snprintf(sqlbuf, 1024, "%d,", 1);
	printf("len=%d, sqlbuf:%s\n", len, sqlbuf);
	len += snprintf(sqlbuf+len, 1024, "%d,", 2);
	printf("len=%d, sqlbuf:%s\n", len, sqlbuf);
	len += snprintf(sqlbuf+len, 1024, "%d,", 3);
	printf("len=%d, sqlbuf:%s\n", len, sqlbuf);

	if(*argv[1] == 'c'){
		sqlite3_exec(db,"create table bList(id integer primary key, fileName varchar(16), binData blob, binData2 blob);",NULL,NULL,&err);

		int i;
		for(i=0;i<10;i++){
			sqlite3_prepare(db,"insert into bList set fileName = 'hex', bindData = ?, binData2 = ?;",-1,&stmt,NULL);
			hex[0] = i;
			sqlite3_bind_blob(stmt,1,hex,16,NULL);
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			printf("insert column %d\n", i);
		}
	}

	if(*argv[1] == 'f'){
		int i=0, id, tlen, blen, blen2;
		char *type, *bin, *bin2;

		sqlite3_prepare(db,"select * from bList;",-1,&stmt,NULL);
		while(SQLITE_ROW  == sqlite3_step(stmt)){
			id = sqlite3_column_int(stmt, 0);
			type = sqlite3_column_text(stmt, 1);
			tlen = sqlite3_column_bytes(stmt, 1);
			bin = sqlite3_column_blob(stmt, 2);
			blen = sqlite3_column_bytes(stmt, 2);
			bin2 = sqlite3_column_blob(stmt, 3);
			blen2 = sqlite3_column_bytes(stmt, 3);
			printf("column %d, id:%d, type:%s, tlen:%d, bin:%x, blen:%d, bin2:%x, blen2:%d\n", i++, id, type, tlen, bin[0], blen, bin2[0], blen2);
		}
		sqlite3_finalize(stmt);
	}

	if(*argv[1] == 's'){
		int i=0, id, tlen, blen;
		char *type, *bin;

		sqlite3_prepare(db,"select * from bList where binData = ?;",-1,&stmt,NULL);
		hex[0] = 5;
		sqlite3_bind_blob(stmt,1,hex,16,NULL);
		while(SQLITE_ROW  == sqlite3_step(stmt)){
			id = sqlite3_column_int(stmt, 0);
			type = sqlite3_column_text(stmt, 1);
			tlen = sqlite3_column_bytes(stmt, 1);
			bin = sqlite3_column_blob(stmt, 2);
			blen = sqlite3_column_bytes(stmt, 2);
			printf("column %d, id:%d, type:%s, tlen:%d, bin:%x, blen:%d\n", i++, id, type, tlen, bin[0], blen);
		}
		sqlite3_finalize(stmt);
	}

	sqlite3_close(db);

	return 0;
}
