/***************************************
*1,创建进程没有最后执行exit注销进程 出现出错信息:process: cxa_atexit.c:100: __new_exitfn: Assertion `l != ((void *)0)' failed.
*2,vfork()后父进程的变量乱码,由于共用栈空间,子进程先完成,然后return退出,释放了栈,导致到父进程执行时,读取栈内容,但已经乱码,
* 1和2的问题,要求vfork的子进程结束进程时要用exit()来退出.
****************************************/


#include <unistd.h>	//fork() vfork() getpid() execl()
#include <sys/types.h>	//vfork() getpid() wait()
#include <stdlib.h>	//exit()
#include <sys/wait.h>	//wait()

#include <stdio.h>


int main (void)
{
	pid_t hpid; //getpid获得
	pid_t pid; //fork返回
	pid_t vpid; //vfork返回

	int c = 0;	//测试栈空间

	/**** getpid ****/
	hpid = getpid();	
	printf("host pid %d\n\r",((int)hpid));
	printf("\n\r");

	/**** fork wait execl****/
	pid = fork();	//创建进程成功,父进程处返回子进程pid,子进程处返回0, 失败返回-1,
			//成功后,实际是把执行代码都拷贝了一份到新的子进程,子进程有自己独立的栈空间,让子进程继续执行余下代码		
	if(pid)		//这里父进程指定执行代码,
	{
		wait(NULL);	//进程等待
				//等待子进程完全结束后再执行父进程,如果子进程还有vfork,也等完子进程的所有vfork进程结束
		hpid = getpid();
		c++;	
		printf("fork parent part host  pid %d\n\rfork parent part child pid %d c:%d\n\r\n\r",	\
			((int)hpid),((int)pid),c);
	}
	else		//这里这里子进程指定执行代码,
	{
		hpid = getpid();
		c++;
		execl("/bin/ls","ls","../",NULL);//文件路径,arg0:执行程序名字这里是ls,arg1:执行程序使用参数"../",没有更多参数时最后一个填NULL
						 //exec最好配合fork()使用,不要配合vfork使用,因为vfork共有了栈区 
		//如果使用了execl(),子进程里所有内容被删掉,被赋予新的内容	
		//原来子进程部分内容也就不再执行,以及子进程后面的vfork也不再执行
		printf("fork child part host  pid %d c:%d\n\r\n\r",	\
			((int)hpid),c);
	}								
#if 1
	/**** vfork exit ****/
	vpid = vfork();	//创建进程成功,会阻塞父进程,即子进程先进行
			//父进程处返回子进程pid,子进程处返回0, 失败返回-1,
			//成功后,实际是把执行代码都拷贝了一份到新的子进程,但子进程与父进程共用栈空间,让子进程继续执行余下代码 		
	if(vpid)		//这里父进程指定执行代码,
	{
		hpid = getpid();
		//c++;	
		printf("vfork parent part host  pid %d\n\rvfork parent part child pid %d c:%d\n\r\n\r",	\
			((int)hpid),((int)vpid),c);
	}
	else		//这里这里子进程指定执行代码,
	{
		hpid = getpid();
		c++;	
		printf("vfork child part host  pid %d c:%d\n\r\n\r",	\
			((int)hpid),c);
		exit(0);	//退出vfork子进程一定要用exit()来执行,不能用后面的 return 0
	}
#endif 

	return 0;
}
