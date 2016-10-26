
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>

int main()
{
    int i=0;
    int ret=0;
    int fd=-1;
    static char buff[8]={'0','0','0','0','0','0','0','0'};
    

    fd=open("/dev/button",0);
    if (fd<0)
    {
        printf("can't open /dev/button\n");
        return -1;
    }
    while(1) {
        ret = read(fd, &buff, sizeof(buff));
        for(i=0; i<8; i++) {
            if (buff[i] == '1')
            printf("Key[%d]press\n",i);
        }
        
    }
        close(fd);
        return 0;
}
