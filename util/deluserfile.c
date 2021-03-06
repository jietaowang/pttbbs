/* 自動砍user目錄檔案程式 */

#include "bbs.h"

#define HOLDWRITELOG
#define DELZEROFILE
#define USERHOME BBSHOME "/home"

void del_file(char *userid)
{
    char buf[200], buf1[200];
    struct dirent *de;
    DIR *dirp;
    char *ptr;

    sprintf(buf, BBSHOME "/home/%c/%s", userid[0], userid);

    if (chdir(buf) == -1)
	return;

    if (!(dirp = opendir(buf)))
	return;

    while ((de = readdir(dirp)))
    {
	ptr = de->d_name;
	if (ptr[0] > ' ' && ptr[0] != '.')
	{
	    if (strstr(ptr, "writelog"))
#ifdef HOLDWRITELOG
	    {
		fileheader_t mymail;

		stampfile(buf, &mymail);
		mymail.filemode = FILE_READ;
		strcpy(mymail.owner, "[備.忘.錄]");
		strcpy(mymail.title, "熱線記錄");
		sprintf(buf1, BBSHOME "/home/%c/%s/writelog",
			userid[0], userid);
		rename(buf1, buf);
		sprintf(buf1, BBSHOME "/home/%c/%s/.DIR", userid[0], userid);
		append_record(buf1, &mymail, sizeof(mymail));
	    }
#else
	    unlink(ptr);
#endif
	    else if (strstr(ptr, "chat_"))
		unlink(ptr);
	    else if (strstr(ptr, "ve_"))
		unlink(ptr);
	    else if (strstr(ptr, "SR."))
		unlink(ptr);
	    else if (strstr(ptr, ".old"))
		unlink(ptr);
	    else if (strstr(ptr, "talk_"))
		unlink(ptr);
	}
    }
    closedir(dirp);
}

void mv_user_home(char *ptr)
{
    char buf[200];

    printf("move user %s to tmp\n", ptr);
    sprintf(buf, "cp -R " BBSHOME "/home/%c/%s " BBSHOME "/tmp", ptr[0], ptr);
//      sprintf(buf,"rm -rf " BBSHOME "/home/%c/%s",ptr[0],ptr);
    if (!system(buf))
    {				//Copy success

	sprintf(buf, "rm -rf " BBSHOME "/home/%c/%s", ptr[0], ptr);
	system(buf);
    }
}

int main(void)
{
    struct dirent *de;
    DIR *dirp;
    char *ptr, buf[200], ch;
    int count = 0;
/* visit all users  */

    printf("new version, deleting\n");

    attach_SHM();

    for (ch = 'A'; ch <= 'z'; ch++)
    {
	if(ch > 'Z' && ch < 'a')
	    continue;
	printf("Cleaning %c\n", ch);
	sprintf(buf, USERHOME "/%c", ch);
	if (!(dirp = opendir(buf)))
	{
	    printf("unable to open %s\n", buf);
	    continue;
	}

	while ((de = readdir(dirp)))
	{
	    ptr = de->d_name;

	    /* 預防錯誤 */
	    if (is_validuserid(ptr))
	    {
		if (!(count++ % 300))
		    printf(".\n");
		if (!searchuser(ptr, ptr))
		    mv_user_home(ptr);
		else
		    del_file(ptr);
	    }
	}
	closedir(dirp);
    }
    return 0;
}
