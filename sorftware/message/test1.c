/****************************************************
*默认一个字符串除了有看得见的内容外,还有一个系统默认加上去的结束符'\0'字节
*所以sizeof()一个字符串会把最后的字节算上,而sizeof其他类型的却不会另外多算上一个结束符
*msqid = msgget(key,flg);	//创建消息队列,成功返回消息队列描述符
*msgsnd(msqid.msgp,msgsz,msgflg)		//往列表写入消息,成功返0,失败返-1,
*					//msgp:指向要发送的消息,
*					//msgsz:消息大小
*					//msgflg:一些标志位.
*msgrcv(msqid.msgp,msgsz,msgtyp,msgflg)	//成功返回实际消息数据的长度(非消息结构体的长度),失败返-1
*					//msgp:存放消息位置,
*					//msgsz:希望取出的消息大小
*					//msgflg:一些标志位.
*					//msgtyp:按消息类型取消息,0,时,忽略类型,直消息队列第一条消息
*					//>0时,取与msgtyp一致的第一条消息
*					//<0时,取消息类型介于(1-|msgtyp|)的消息,且,取消息类型值最小的第一条消息
*msgctl(msgid,IPC_RMID,*buf)		//删除消息队列.
*					//*buf  获取内核中的msqid_ds结构,每个消息队列都有一个msqid_ds,存储相关信息,一般用不到
*
*调试:
*	write read都要用root运行,
*	父进程不能结束,否则,子进程都变成了后台进程,而且父进程用return离开,影响子进程获取参数
*	一个父进程fork多个子进程,父进程使用wait函数有bug, 当有子进程退出时会辗转建立多个进程又删除,折腾一轮才结束.
*
*
*****************************************************/

#include <sys/types.h>	//msgget() msgsnd() msgrcv() msgctl() ftok() getpid()
#include <sys/ipc.h>	//msgget() msgsnd() msgrcv() msgctl() ftok()
#include <sys/msg.h>	//msgget() msgsnd() msgrcv() msgctl()


#include <string.h>   //strncpy() strncmp()

#include <stdio.h>   //printf() fgets()
#include <stdlib.h>  //exit() scanf()

#include <unistd.h>  //sleep() fork() getpid()

#define MSG_SIZE	128	//消息内容大小
	


//自定义一个消息结构
struct msg_str
{
	long msgtp;		//消息类型
	char msg[MSG_SIZE];	//消息内容
};

struct msg_str mesg;
int msgid;		//消息队列描述符
ssize_t ret;

//子进程
void childprocess(void)
{
	pid_t cpid;
	
	cpid = getpid();

	while(1)
	{
		ret = msgrcv(msgid,&mesg,sizeof(struct msg_str),0,0);	//取出第一条消息
		if (ret == -1)
		{
			printf("child:%d\n",cpid);	//打印
			perror("msgrcv:");

			return;		//出错就结束.
		}
		printf("child:%dmessage type:%ld,message:%s\n",cpid,mesg.msgtp,mesg.msg);	//打印
		//sleep(10);		//通过sleep发现只有一个子进程可以使用msgrcv函数.

	}
	return;
}


int main(void)
{
	key_t key;		//键值 详细描述可见semaphore

	int msgtp1;		//存放消息类型的缓存
	char msg1[MSG_SIZE];	//存放消息内容的缓存

	struct msg_str mesg;

	pid_t pid;		//进程pid

	int running = 1;	//循环测试标记
	int i;


	key = ftok("/home/kingders/smart210/sorftware/message",1);	//创造一个键值,
	if (key == -1)						
	{
		printf("get key fair!!\n\r");
		exit(EXIT_FAILURE);
	}

	msgid = msgget(key,IPC_EXCL);	//打开一个存在消息队列
	if (msgid == -1)						
	{
		printf("open message_list fair!!\n\r");
		exit(EXIT_FAILURE);
	}

#if 1
	for(i=0;i<8;i++)
	{
		pid = fork();
		if(pid < 0)
			printf("fork child_process fail\n");
		else if (pid == 0)	//子进程
			childprocess();	
		else 
		{	
			//while(1);	//父进程不能自己先结束
			wait(NULL);	//父进程等待所有子进程结束
		}
	}
#endif

	pid = getpid();
	printf("parent:%d\n",pid);	//打印
	return 0;
}






























