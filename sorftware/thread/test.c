/*************************************************
*使用线程时,编译时必须链接到pthread库: gcc -lpthread
*线程退出不能用exit,否则,其所属于整个进程,和进程创建的线程们都退出,新城退出使用:pthread_exit
*
*
*pthread_create(*thread,*attr,函数指针,void *arg)	//创建新的线程, 成功返回0,失败返回错误编码.
*						//*thread:新线程的id最后会存在这
*						//*attr:linux系统待创建线程的的属性,NULL时,使用默认属性
*						//线程执行的函数,线程创建后,直接从这个函数开始执行
*						//*arg:线程入口函数的参数,可以为空
*
*pthread_join(*thread,**retval)			//进程等待所创建的线程结束,成功返0,失败返错误编号
*						//*thread:线程的id
*						//**retval保存线程退出时状态,NULL为不保存
*pthread_exit(*retval)				//线程退出,函数没有返回值,*retval保存线程返回值.
*
*
*
*pthread_tex_init()				//创建互斥锁
*pthread_tex_lock()				//获取互斥锁
*pthread_tex_unlock()				//释放互斥锁
*
*
*pthread_cond_t condready = PTHREAD_COND_INITIALIZER	//定义一个条件变量
*pthread_cond_wait()				//等待条件,线程b执行时,直接睡眠
*pthread_cond_signal()				//通知条件,线程a执行时,内核调用会荤腥线程b执行
*
*
*
*模型:
*thraad1 重复生产产品,每生产一个,休息1秒,
*thread2 检查是否累计已有5个A产出,若是,就消费掉.若不是就休息2秒
*包含了同步操作,就是2一定要等1生产5个产品后才会去消费,
*	方案一:没有使用条件变量,使用蠢方法实现同步操作,同时方案一没有考虑用互斥锁保护共用资源produc_count,但也不需要.
*	      添加互斥锁保护一个全局累加器,每个线程加一后显示当前值,保证同一个值不被显示两次,
*	      (如果不保护,有可能,线程1加一后,还没有printf,线程2也加了一,最后线程12都printf了同一个值)
*	方案二:使用条件变量代替 方案一 实现同步操作.
*	       原理: 线程2检测是否满5个产品,不满足就沉睡,线程1做满5个产品便唤醒线程2,
*		     线程2唤醒后并不直接取得睡眠时释放的互斥锁,而是要重新竞争获得,
*		     为了保证线程2唤醒后便获得锁,得尽量避免此时有其他线程与线程2竞争锁.
*	       最后,尽量保证 pthread_cond_wait() 先于 pthread_cond_signal() 执行!!
*
*测试内容:1,基本线程工作 (实现)
*	 2,使用互斥锁   (实现)
*	 3,使用条件变量 (实现)
*	 4,线程参数,以及返回值 (测试返回 出现了Segmentation fault (core dumped) 错误)
*	   用这个错误测试其他纠错工具 gdb core文件等等!!
*
*
*
*编译调试:
*	1,当线程执行函数格式非常严格:void *thread1(void *data) //返回值是空类型指针,参数是空类型指针参数,尽管我们不需要参数
*	  如果,这样:void *thread1(void),则会编译出错(虽然实际上这样更符合我们不需要参数的情形):
*		warning: passing argument 3 of ‘pthread_create’ from incompatible pointer type [enabled by default]
*	2,编译命令: gcc -pthread test.c -o test 才正确, 通过man pthread_create 发现
*	   gcc -lpthread test.c -o test 错误,-lpthread 是古老版本库文件的编译项,现在用不着
*	3,测试线程2不能正常返回值,然贺实现返回方式与线程1无疑.	printf("%s\n",*rcv2);出现:Segmentation fault (core dumped)错误
*
*
*
*
*
**************************************************/
#include <pthread.h>

#include <sys/types.h>	//ftok()
#include <sys/ipc.h>	//ftok()

#include <string.h>   //strncpy() strncmp()

#include <stdio.h>   //printf() fgets ()
#include <stdlib.h>  //exit() scanf()
#include <string.h>  //strlen()

#include <unistd.h>  //sleep()

//#include <linux/kprobes.h>	//dump_stack()
//#include <asm/traps.h>	//dump_stack()


pthread_t thread[2];		//线程描述符
int produc_count = 0;		//累计生产数量
pthread_mutex_t	mut;		//一个互斥锁
int plus = 0;			//全局累加器,只用于方案一的测试
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;		//初始化一个条件变量值,PTHREAD_COND_INITIALIZER是一个初始值


void *thread1(void *data)
{
	char *s1 = "thread1 return mio";
	printf("%s\n",(char*)data);	//测试线程的参数传递
#if 0	//方案一

	int i = 0;
	char a = 'a';

	while(1)
	{

		//模型的主题内容
		if(produc_count == 5)			//如果生产了的5个产品未被thread2消费,就暂停生产
			continue;
		if( i == 20)				//累计生产了20次就不再生产而结束
			break;	
		i++;
		produc_count++;
		printf("thread1: product %c\n",(char)(a+i));	//char可以不转型与int做加减
		if( !( produc_count % 5 ) )
		{
			printf("thread1: have product 5\n");
		}	
		sleep(1);

		//附加内容:测试互斥锁,用累加器
		pthread_mutex_lock(&mut);			//保护累加器plus
		plus++;
		printf("t1: + %d\n",plus);
		pthread_mutex_unlock(&mut);			//结束保护
	}

#endif	//方案一	

#if 1	//方案二

	int i = 0;
	char a = 'a';

	while(1)
	{
		if( i == 20)				//累计生产了20次就不再生产而结束
			break;

		pthread_mutex_lock(&mut);			//保护produ_count
		produc_count++;
		printf("thread1: product %c\n",(char)(a+i));	//char可以不转型与int做加减
		if(produc_count == 5)			//如果生产了的5个产品未被thread2消费,就通知使用cond等待的线程
			pthread_cond_signal(&cond);	//由于条件唤醒的线程是重新竞争取得互斥锁,所以不一定取得锁
							//所以在线程1还没有释放锁前,就唤醒线程2,让线程2已经优先阻塞在取锁的队列上
							//然而后面发现即使唤醒了,线程二并没有阻塞在取锁的前头队列上,
							//而是等待锁释放,然后又白痴地跟线程1竞争锁
							//所以保证线程2能优先取得锁的途径为,线程1,在线程2想取锁的时候sleep段时间
		i++;
		if( !( produc_count % 5 ) )
		{
			printf("thread1: have product 5\n");
		}
		pthread_mutex_unlock(&mut);			//结束保护 注意不要在sleep后释放锁,要此刻就是释放锁.	
		sleep(1);
		//pthread_mutex_unlock(&mut);			//结束保护 这里释放的话,线程1一转眼有竞争到锁,完全不给线程2机会会
	}
#endif	//方案二
	
	printf("thread1:over \n"); //线程退出前消息
	//pthread_exit(NULL);	//线程退出 
	pthread_exit(s1);	//线程退出,返回返回值
}

void *thread2(void *data)
{
	char *s2 = "thread2 return yui";
	printf("%s\n",(char*)data);	//测试线程的参数传递
#if 0	//方案一

	int j = 0;
	char b = 'a';

	while(1)
	{
		//模型的主题内容
		if (j==20)		//消费5次后退出线程
			break;
		
		if( produc_count == 5)
		{
			produc_count = 0;	//清空计数器
			j += 5;
			printf("thread2: consume %c%c%c%c%c\n",		\
				(char)(b+j-5),(char)(b+j-4),(char)(b+j-3),(char)(b+j-2),(char)(b+j-1));	//char可以不转型与int做加减
		}
		else
		{
			printf("thread2: wait product \n");
			sleep(2);
		}	

		//附加内容:测试互斥锁,用累加器
		pthread_mutex_lock(&mut);			//保护累加器plus
		plus++;
		printf("t2: + %d\n",plus);
		pthread_mutex_unlock(&mut);			//结束保护		
	}

#endif	//方案一	

#if 1	//方案二

	int j = 0;
	char b = 'a';
		
	while(1)
	{
		if (j==20)		//消费5次后退出线程
			break;
		pthread_mutex_lock(&mut);			//保护produc_count,不被修改
		if( produc_count != 5)
		{
			printf("thread2: cond_wait product \n");
			pthread_cond_wait(&cond, &mut);		//当produc_count不满足特定情况,我们让线程2睡眠等待,并会释放互斥锁,
								//被唤醒后,会重新竞争获取互斥锁,所以并不一定取得锁
		}
		//被唤醒后,又竞争锁后的工作,		
		produc_count = 0;	//清空计数器
		j += 5;
		printf("thread2: consume %c%c%c%c%c\n",		\
		(char)(b+j-5),(char)(b+j-4),(char)(b+j-3),(char)(b+j-2),(char)(b+j-1));	//char可以不转型与int做加减
		sleep(2);		//测试 被唤醒后,会重新上回互斥锁
		pthread_mutex_unlock(&mut);			//结束保护		
	}
#endif	//方案二	

	printf("thread2:over \n"); //线程退出前消息
	//pthread_exit(NULL);	//线程退出
	pthread_exit(s2);	//线程退出,返回返回值
}


int main (void)
{
	char *string1 ="thread1 use for product";
	char *string2 ="thread2 use for consume";

	char **rcv1;
	char **rcv2;
	
	pthread_mutex_init(&mut,NULL);			//初始化互斥锁 

	//pthread_create(&thread[0],NULL,thread1,NULL);	//创建线程thread1
	//pthread_create(&thread[1],NULL,thread2,NULL);	//创建线程thread2

	pthread_create(&thread[0],NULL,thread1,(void*)string1);	//创建线程thread1,并传递了参数
	pthread_create(&thread[1],NULL,thread2,(void*)string2);	//创建线程thread2.并传递了参数

	//pthread_join(thread[0],NULL);			//等待thread1结束
	//pthread_join(thread[1],NULL);			//等待thread2结束

	pthread_join(thread[0],(void **)rcv1);			//等待thread1结束,并得到返回值放在 **rcv1
	pthread_join(thread[1],(void **)rcv2);			//等待thread2结束,并得到返回值放在 **rcv2

	printf("%s\n",*rcv1);	//测试线程1返回值
	printf("%s\n",*rcv2);	//测试线程2返回值

	printf("over\n");		//进程结束前标记
	return 0;	
}
























