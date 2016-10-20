//#include <pthread.h>

//#include <sys/types.h>	//ftok()
//#include <sys/ipc.h>	//ftok()

#include <string.h>   //strncpy() strncmp()

#include <stdio.h>   //printf() fgets ()
#include <stdlib.h>  //exit() scanf()
#include <string.h>  //strlen()

#include <unistd.h>  //sleep()

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


void main (void)
{
	int i;
 
	if ( fork() == 0)
	{
		printf("\nsetsid\n\r");
		//setsid();
		chdir("/");
		for(i=0;i<10;i++)
		{		
			printf("daemon\n\r");
			sleep(2);
		}
		exit(1);	
	}
	exit(1);

	//return 0;
}
