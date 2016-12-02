
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>

int main()
{
    int i = 10;
    int fd = -1;
    int ret = 0;
    int c;
    

    fd=open("/dev/button",0);
    if (fd<0)
    {
        printf("can't open /dev/button\n");
        return -1;
    }
    while(i--) {
        ret = read(fd, &c, 4);
        printf("c is: %d\n",c);

    }
        close(fd);
        return 0;
}
