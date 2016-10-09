/*********************************************************
*kill()是发送信号函数,不是特指发送 kill信号的函数
*main函数的参数:int argc 参数的个数,
*		char *argv[] 真正的参数 一组指向字符串的数组指针
*注意:argv[0]:存放指向 这个执行程序文件名 的指针
*     argv[1]:存放指向 第一个字符串参数 的指针
*	...
*
*测试:
*ps aux		//查到要接受信号的进程的pid 为25719
*./kill 25719	//这个shell命令,传递给kill程序参数: argv[0]:存放指向字符串 "./kill" 的指针,argv[1]:存放指向字符串 "25719" 的指针
*
*
*
*
*********************************************************/

#include <signal.h>	//kill()
#include <sys/types.h>	//kill()

#include <stdio.h>



int main (int argc,char *argv[])	//需要使用参数
{
	pid_t pid;
	pid = atoi(argv[1]);

	kill(pid,SIGINT);

	return 0;
}
