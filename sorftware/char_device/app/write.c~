#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main (void)
{
	int fd = 0;
	int dat = 7982;

	fd = open("/dev/chardev0",O_RDWR);
	perror("open");
	write(fd,&dat,sizeof(int));
	perror("write");
	//printf("write:dat is %d\n",dat);
	printf("writing!!\n");
	
	close(fd);
	return 0;
}
