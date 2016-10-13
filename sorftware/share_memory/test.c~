/****************************************************
*
*
*
*
*shmat(shmid,shmaddr,shmflg)	//这里是把共享内存映射到用户进程的内存上来,成功返回映射到的用户内存的位置地址,不成功返-1.
*				//shmid共享内存描述符, shmflg共享内存方式标记
*				//共享内存指定映射到用户进程的内存地址shmaddr,无需特定的填NULL
*				//shmflg共享内存方式标记,无需特殊方式的填0.
*
*****************************************************/

#include <sys/ipc.h>   //shmget() ftok()
#include <sys/shm.h>   //shmget() ftok() shmat() shmdt()
#include <sys/types.h> //ftok() shmat() shmdt()

#include <string.h>   //strncpy() strncmp()

#include <stdio.h>   //printf() fget()
#include <stdlib.h>  //exit()

#include <unistd.h>  //sleep()


#define TXT_SIZE	2048		//2kb空间


//自定义一个共享内存的结构
struct shm {
	int flag;	//自定义一个记录内存状态信息的flag,这里的flag简单,只记录是否有内容.
	char txt[TXT_SIZE];	//具体存放的内容
};


int main(void)
{
	key_t key;		//键值 详细描述可见semaphore
	int shmid;		//共享内存描述符
	struct shm *shm1;	//映射到共享内存 的这个进程自身的内存位置
	char buffer[TXT_SIZE];	//输入缓存
	int running = 1;	//循环测试标记


	key = ftok("/home/kingders/smart210/sorftware/share_memory",1);	//打开一个键值,
	if (key == -1)						
	{
		printf("creat key fair!!\n\r");
		exit(EXIT_FAILURE);
	}

	shmid = shmget(key,sizeof(struct shm),IPC_CREAT);	//打开一个共享内存,sizeof()算出了共享内存的大小
	if (shmid == -1)						
	{
		printf("creat share_memory fair!!\n\r");
		exit(EXIT_FAILURE);
	}

	shm1 = (struct shm *)shmat(shmid,NULL,0);	//映射共享内存,不用特别指向用户内存的某具体地址,也没有其他特殊,选默认方式

	//测试共享内存工作 写入工作
	while (running)
	{
		while(shm1->flag)			//检查共享内存内容是否已经读出
		{
			sleep(1);
			//printf("wait");
		}
		
		fgets(buffer,TXT_SIZE,stdin);		//从键盘(标准输入stdin)上获取数据 到输入缓存,当得到回车符号算输入结束.
		strncpy(shm1->txt,buffer,TXT_SIZE);	//复制到共享内存.
		shm1->flag = 1;				//标记共享内存有内容

		if(strncmp(buffer,"end",3) == 0)	//发现输入内容有end,便结束测试.
			running = 0;

	}
	
	shmdt((const void *)shm1);		//解除与共享内存的映射;

	return 0;
}

