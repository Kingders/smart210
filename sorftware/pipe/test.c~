/***************************************
*由此对pipe深刻认识:
*当前进程可以自己自己写信息到管道,又继续自己读回
*父进程fork时会管道的读写文件描述符都给子进程拷贝一份
*父子进程使用管道时,各自关闭其中一个端口,一个管道只能单向传递,但是都不关闭时,可以按需要形成双向传递
*
*管道本质:拥有某个管道fd[0]的进程都有机会从管道读出数据,拥有fd[1]的进程都有机会写入数据,
*	  但由于fd[0]fd[1],只能通过fork()继承,只能服务与父子进程间数据传输.
*
****************************************/


#include <unistd.h>	//fork() vfork() getpid() execl()	//pipe()
#include <sys/types.h>	//vfork() getpid() wait()		//
#include <stdlib.h>	//exit()				//
#include <sys/wait.h>	//wait()				//

#include <stdio.h>

#define MSDATA		"hello"		//父给子进程数据
#define MSDATA_SIZE	sizeof(MSDATA)		//计算字符串字节数,自动多算一个字符串结束符NULL

#define SMDATA0		"jiuhygfd"		//子给父进程数据
#define SMDATA1		"hahahaha"		//子给父进程数据
#define SMDATA2		"bbbbbbbb"		//子给父进程数据
#define SMDATA_SIZE	sizeof(SMDATA0)		//计算字符串字节数,自动多算一个字符串结束符NULL

int main (void)
{
	pid_t hpid; //getpid获得
	pid_t pid; //fork返回
	pid_t vpid; //vfork返回
	char rbuffer[20];

	int pipefd[2];	//pipefd[0]管道读端文件描述符 pipefd[1]管道写端文件描述符

	/**** getpid ****/
	hpid = getpid();	
	printf("host pid %d\n\r",((int)hpid));
	printf("\n\r");

	/**** pipe() ****/
	pipe(pipefd);	//创建管道,要在fork()创建进程前,先在父进程创建
			//实际创建管道文件,并打开两次文件,一次读端模式,一次写端模式.记录两个文件描述符	
	pid = fork();	//创建进程成功,父进程处返回子进程pid,子进程处返回0, 失败返回-1,
	//fork()后未知父子进程谁先执行,这里fork后多为父进程先执行,假设父进程先执行,
	//父进程 -> 写入有 hello\0 共6字节信息 -> wait(转到子进程) -> 
	//子进程 -> 读出 6字节数据到rbuffer显示 -> 写入有 jiuhygfd\0 共9字节信息->写入有 hahahaha\0 共9字节信息
	//	 -> 读出 9字节数据到rbuffer显示 -> wait(转回父进程) ->
	//父进程 -> 读出 9字节数据到rbuffer显示 -> 读出 9字节数据到rbuffer显示,但由于此时管道文件已空,所以阻塞等待,转到子进程 ->
	//子进程 -> 写入有 bbbbbbbb\0 共9字节信息 -> return 0 结束子进程 (返回父进程)
	//父进程 -> 继续完成读出 9字节数据到rbuffer显示 -> return 0 结束父进程
	if(pid)		//这里父进程指定执行代码,
	{
		//close(pipefd[0]);	//并一定不需要关闭读端
		hpid = getpid();
		printf("fork parent part host  pid %d\n\rfork parent part child pid %d\n\r\n\r",	\
			((int)hpid),((int)pid));
		//write_pipe
		write(pipefd[1],MSDATA,MSDATA_SIZE);
		wait(NULL);
		read(pipefd[0],rbuffer,SMDATA_SIZE);	
		printf("parent read:%s\n\r",rbuffer);
		read(pipefd[0],rbuffer,SMDATA_SIZE);	
		printf("parent read:%s\n\r",rbuffer);
	}
	else		//这里这里子进程指定执行代码,
	{
		//close(pipefd[1]);	//并一定不需要关闭写端
		hpid = getpid();
		printf("fork child part host  pid %d\n\r",((int)hpid));
		read(pipefd[0],rbuffer,MSDATA_SIZE);
		printf("child read:%s\n\r",rbuffer);
		printf("ok\n\r");
		write(pipefd[1],SMDATA0,SMDATA_SIZE);
		write(pipefd[1],SMDATA1,SMDATA_SIZE);
		read(pipefd[0],rbuffer,SMDATA_SIZE);
		printf("child read:%s\n\r",rbuffer);
		wait(NULL);
		write(pipefd[1],SMDATA2,SMDATA_SIZE);
	}								


	return 0;
}
