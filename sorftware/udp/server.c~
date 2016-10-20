/***************************************************



socket(domain,type,protocol);		//创建套接字特殊文件,返回套接字文件描述符,创建后,套接字自动绑有一个随机的ip地址
					//domain:例如,AF_INET指1pv4协议
					//type:例如SOCK_DRGAM,长度国定,不可靠报文形式.
					//domain,type,protocol共同决定socket履行什么样的网络协议
					//例如:domain:AF_INET,type:SOCK_DRGAM,protocol:默认,则socket履行UDP协议
					//例如:domain:AF_INET,type:SOCK_STREAM,protocol:默认,则socket履行TCP协议

bind(sockid,struct sockaddr,)					//给创建好的套接字重新绑定一个指定ip,



struct sockaddr{			//通用地址结构体
	sa_family_t sa_family;		//地址的类型AF_INET还是AF_INET6...
	char sa_data[14];		//填写地址的空间
}
struct sockaddr_in{			//ipv4地址结构体,与通用地址结构体大小一样
	short int sa_family;		//地址的类型AF_INET还是AF_INET6...
	unsigned short int sin_port;	//端口
	struct in_addr sin_addr;	//ip addr
	unsigned char sin_zero[8]	//填0的不用区域
}
struct in_addr{				//存放ip addr
	unsigned long s_addr;
}



//地址转换 
in_addr_t inet_addr(const char *cp);	//把一个字符串地址例"192.168.1.1"转成数型,而且也自动转成网络字节序
char* inet_ntoa(struct in_addr);	//把一个整数地址转成字符串型


listen()				//让socket文件准备好接受客户端访问.

accept()				//等待客户端连接,执行时,如果没有客户端连接时,阻塞服务器程序,等待直到有访问.
					//成功等到后,返回一个新的socket套接字文件描述符.作为与这个客户端的专门通道.


connect					//客户端的专门函数,连接服务端


字节序
小端模式:数据从低地址开始放,
大端模式:数据从高地址开始放
网络发送数据按大端模式,也称为网络字节序

如果机子属于小端模式,通过网络发数据时,要都先转成大端模式(网络字节序)
即主机字节序转成网络字节序 

htonl(uint32_t hostlong)	//32位数据从主机字节序转成网络字节序  (假设主机字节序是小端模式)
htonl(uint16_t hostshort)	//16位数据从主机字节序转成网络字节序  

ntohl(uint32_t hostlong)	//32位数据从网络字节序转成主机字节序  (假设主机字节序是小端模式)
ntohs(uint32_t hostlong)	//16位数据从网络字节序转成主机字节序


调试问题:
	1,由于是自定义的udp程序,所以服务端的端口 不能选择一些被占用,或者国际规定的端口,不然connect显示 connection refused!!
	2,此外关于连接问题:
		udp下,套接字不需要listen 和aceept 来建立握手连接,直接使用recvfrom 进行单次的数据接收便可


********************************************************************/
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



#define portnum		4444		//自定义的一个端口,注意不能选低于1024的端口号,因为都是正式的国际规定的专用端口,不能用


int main (void)
{
	int sockfd,sockfd1;
	struct sockaddr_in server_addr;			//存储ipv4网络地址数据的结构,而struct sockaddr是存储通用网络地址数据的结构
	struct sockaddr_in client_addr;	
	char rbuf[128];
	int nbyte;					//一次接收函数实际接收的字节数
	socklen_t sockaddr_len = sizeof(struct sockaddr);



	
	//先创建一个套接字
	if ((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1 )	//执行udp协议的套接字,protocol可默(认为0
	{							//生成的套接字文件可看作是一个打开了的特殊的文件,所以成功时,返回文件描述符
		printf("create socket fair\n");			//生成的socker文件,会自动绑定一个自动分配的地址
		exit(1);
	}

#if 1	//地址绑定,方案一
	
	//由于作为服务器服务,所以要求重新绑定一个自定义统一的地址,而不是其本来未知的自动分配地址
	//而且一个socker可以绑定多个地址,意味着可以通过访问多个不同地址访问到这个服务器的socker上来
	//这里ipaddr设 INADDR_ANY 表示,客户端访问任意地址都可以访问到这个服务器上的socker来
	bzero(&server_addr,sizeof(struct sockaddr_in));				//清零server_addr这个结构数据变量
	//填写待绑定地址信息
	server_addr.sin_family = AF_INET;		//填写类型
	server_addr.sin_port = htons(portnum);		//填写端口,转为网络字节序才可以赋值使用
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 	//填写地址,转为网络字节序才可以赋值使用
	//server_addr.sin_addr.s_addr = inet_addr("192.168.0.107"); 	//填写地址,inet_addr:字符串转成整形,并自动转为网络字节序,
	//server_addr.sin_addr.s_addr = inet_addr("192.168.0.116"); 	//填写地址,inet_addr:字符串转成整形,并自动转为网络字节序,
	bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr)); //绑定

#endif	//地址绑定,方案一

	while(1)
	{	
		//清空rbuf
		bzero(rbuf,128);				
		//等待接收数据,并阻塞进程,成功后存储在rbuf,一次接收最大字节数128,而实际这次接收收到多少字节会存在nbyte里,
		nbyte = recvfrom(sockfd,rbuf,128,0,(struct sockaddr *)(&client_addr),&sockaddr_len);	
		printf("connect from client:%s, socker_fd:%d\n",inet_ntoa(client_addr.sin_addr),sockfd);
		rbuf[nbyte] = '\0';			//给接收到的字符串(我们测试例子里只传输字符串)补结束符号
		printf("receive:%s\n",rbuf);

		//关闭连接,其实就是把连接好的套接字文件关闭掉,套接字文件关闭后会自动被删除
		close(sockfd1);
	}

	//关闭整个服务器连接服务`,其实就是,关闭用来监听的套接字
	close(sockfd);

	return 0;
}






































































































