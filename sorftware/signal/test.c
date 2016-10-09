/*********************************************************
*把int转化成一个函数指针容易,但以同样方式把函数指针转成int却失败 即 ((int)myfunc) 这样的,并不允许
*打印一个函数的函数指针: printf("myfunc:%p\n\r",myfunc);
*handler = signal(SIGINT,myfunc); 注册了一个signal处理,信号处理函数是myfunc,但是返回的函数指针却不是指向myfunc的函数指针	
*此外,signal第二个参数不一定是 自定义信号处理函数的指针,也可以:
*		SIG_IGN  收到信号后,不作为, 
*		SIG_DFL	 收到信号后,执行系统默认信号处理函数
*测试:
*ps aux	查看进程的pid
*kill -n SIGINT 15682	15682是一个要接收信号的进程的pid
*
*
*
*
*********************************************************/


#include <signal.h>	//signal()
#include <unistd.h>	//pause()

#include <stdio.h>

typedef void (*sighandler_t)(int);	//这里要注册 sighandler_t 类型,才能使用
					//虽然man 里 signal()函数的一个参数和返回值都是这个sighandler_t 类型
					//但是实际上 signal()函数的一个参数和返回值都是这个__sighandler_t 类型
					//要注册了"GPL"后才可以不用注册而使用
sighandler_t handler;			
//__sighandler_t handler;		//如果不注册sighandler_t,可以直接使用__sighandler_t代替

void myfunc(int a)
{
	printf("process test receive SIGINT\n\r");
}

int main (void)
{
	int b;
	handler = signal(SIGINT,myfunc);	
	
	//检测signal()函数返回的函数指针,是否等于信号处理函数myfunc的指针
	printf("handler:%p\n\r",handler);
	printf("myfunc:%p\n\r",myfunc);

	//不相关内容,主要,分析函数指针问题
	//handler = myfunc;		//这里把myfunc函数指针赋值给handler
	//handler(1);			//这里通过handler执行myfunc函数

	pause();	//暂停进程	

	return 0;
}
