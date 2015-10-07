#include <stdio.h>
#include <unistd.h>
#include <mntent.h>

#define MTAB "/etc/mtab"

int main(int agrv, char *argc[])
{
	struct mntent *mntent;
	FILE *file = NULL;

	file = setmntent(MTAB, "r");
	if (!file) {
		printf("failed to open %s", MTAB);
		return -1;
	}

	while ((mntent = getmntent(file))) {
		printf("%s, dir:%s, type:%s, opts:%s, freq:%d, passno:%d\n",
				mntent->mnt_fsname, mntent->mnt_dir, mntent->mnt_type, mntent->mnt_opts,
				mntent->mnt_freq, mntent->mnt_passno);
	};

	return 0;
}
