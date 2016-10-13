
#include <sys/types.h>	//semget() semop()
#include <sys/ipc.h>	//semget() semop()
#include <sys/sem.h>	//semget() semop()

#include <stdio.h>
#include <fcntl.h>

#define MES1	"first message from slave\n\r"
#define MES2	"second message from slave\n\r"

int main (void)
{
	int fd;
	key_t key;
	int semid;
	struct sembuf sops[2];
	int semval0,semval2;			//检查信号量的值
	int ret;				//检查信号量的操作函数状态.

	key = ftok("/home/kingders/smart210/sorftware/semaphore",1);	//由于writeboard已创建,这里是得到一个键值,

	semid = semget(key,3,IPC_CREAT);				//由于已创建了3个信号量的,这里是提取信号量集合

	//检查信号量初始值,
	semval0 = semctl(semid,0,GETVAL);
	if(semval0 == -1)
		perror("semctl_getvalue");
	semval2 = semctl(semid,2,GETVAL);
	if(semval2 == -1)
		perror("semctl_getvalue");
	printf("semval0:%d,semval2:%d\n\r",semval0,semval2);
	
	fd = open("./board.txt",O_RDWR|O_APPEND);			//并不执行没有时新建,所以自己人为先建一个

	//获取信号量
	sops[0] =(struct sembuf){.sem_num = 2,
		   .sem_op = -1,
		   .sem_flg= SEM_UNDO};
	sops[1] = (struct sembuf){0,-1,SEM_UNDO};
	ret = semop(semid,sops,2);			//这里操作获取两个信号量
	if(ret == -1)
		perror("semop_getsem");
	printf("ret:%d\n\r",ret);


	//写文件内容
	write(fd,MES1,sizeof(MES1)-1);		//与读写管道不一样,一个文件里不应该有'\0','\0'是系统自动为文件结尾添加,不用人为写入
						//所以sizeof(MES1)-1,因为sizeof自动算上'\0',write也会因为写入字节比实际字节多一个,
						//而最后把'\0'写入
	write(fd,MES2,sizeof(MES2)-1);

	//释放两个信号量
	sops[0].sem_op = 1;	
	sops[1].sem_op = 1;
	ret = semop(semid,sops,2);
	if(ret == -1)
		perror("semop_freesem");

	close(fd); 


	return 0;
}







