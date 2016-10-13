/****************************************************
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
*
*
*
*
*****************************************************/

#include <sys/types.h>	//msgget() msgsnd() msgrcv() msgctl() ftok()
#include <sys/ipc.h>	//msgget() msgsnd() msgrcv() msgctl() ftok()
#include <sys/msg.h>	//msgget() msgsnd() msgrcv() msgctl()


#include <string.h>   //strncpy() strncmp()

#include <stdio.h>   //printf() fgets ()
#include <stdlib.h>  //exit() scanf()
#include <string.h>  //strlen()


#include <unistd.h>  //sleep()

#define MSG_SIZE	128	//消息内容大小

//自定义一个消息结构
struct msg_str
{
	long msgtp;		//消息类型
	char msg[MSG_SIZE];	//消息内容
};


int main(void)
{
	key_t key;		//键值 详细描述可见semaphore
	int msgid;		//消息队列描述符
	int msgtp1;		//存放消息类型的缓存
	char msg1[MSG_SIZE];	//存放消息内容的缓存
	char *for_c;		//用来统计字符串

	struct msg_str mesg;

	int running = 1;	//循环测试标记


	key = ftok("/home/kingders/smart210/sorftware/message",1);	//创造一个键值,
	if (key == -1)						
	{
		printf("creat key fair!!\n\r");
		exit(EXIT_FAILURE);
	}

	msgid = msgget(key,IPC_CREAT);	//创建一个消息队列
	if (msgid == -1)						
	{
		printf("creat message_list fair!!\n\r");
		exit(EXIT_FAILURE);
	}


	//测试消息队列 
	while (running)
	{
		printf("enter message type,0 for quit\n");
		scanf("%d",&msgtp1);		//获取消息类型 第二个参数一定要指针格式,当用户键盘输入不是纯数字字符,函数会出错返回
		if(msgtp1 == 0)			//如果类型为 0 ,退出循环
			break;

		printf("enter message,\n");
		scanf("%s",msg1);		//获取消息内容
		
		//构建消息
		mesg.msgtp = msgtp1;
		strcpy(mesg.msg,msg1);		//在这里认为字符串可以被空格分割,所以一条信息里不能有空格,而且最好不用strncpy,

		//额外内容.sizeof函数, strlen
		//在统计字符串长度,函数中strlen更易用,但是不自动算入字符串的"\0"结束符	
		//for_c = msg1;
		for_c = "hjkl";
		//printf("sizeof(msg1)%ld,sizeof(mesg.msg)%ld,sizeof(for_count)%ld\n",sizeof(msg1),sizeof(mesg.msg),sizeof(for_c));
		printf("strlen(msg1)%ld,strlen(mesg.msg)%ld,strlen(for_count)%ld\n",strlen(msg1),strlen(mesg.msg),strlen(for_c));
		printf("%s\n",for_c);

		msgsnd(msgid,&mesg,sizeof(struct msg_str),0);	//发送消息到队列,最后参数为0,表示默认方式 

	}

	msgctl(msgid,IPC_RMID,0);		//删除消息队列,最后参数为0,表示默认方式 

	return 0;
}






























