/***************************************************

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
	1,由于是自定义的tcp程序,所以服务端的端口 不能选择一些被占用,或者国际规定的端口,不然connect显示 connection refused!!
	2,此外关于对服务器地址信息的填写问题:
		1,server_addr.sin_addr.s_addr = inet_addr("192.168.0.xxx");
			自动把远程服务器的的网卡的地址字符串格式转成系统使用的整型格式,并换成了网络字节序格式再填入
		2,server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
			INADDR_ANY不是针对客户端使用的,这样的话,这样填写并不正确,非要这样的话,相当于远程服务器的的网卡是自身回环网卡,
			相当于:server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
			INADDR_ANY 与 广播连接 完全没有关系.
		3,另外对网卡也有更深的认识,网卡的发送和接收是两个独立的模块,
		  	如果在同一个pc执行 server和client 且pc通过以太网卡/无线网卡连上路由器,
			client 设置远程的服务器网卡地址和服务端口都是自身的以太网卡/无线网卡,
			则client发送连接请求,会从 自身以太网卡/无线网卡发送端 -> 路由器 ->自身以太网卡/无线网卡接收端
			从而与pc自身运行的server程序建立tcp连接,并能正常工作,相当于使用回环网卡一样.
			至于server和client程序使用相同ip地址的理解为,使用同一个网卡!!
			而这个网卡在局域网里是唯一的,即局域网里不存在两个使用相同ip地址的网卡
			而server和client程序使用相同ip地址冲突理解为:server和client程序在不同pc上运行,而这两个pc的网卡有一样的ip地址,
			这样局域网里存在两个使用相同ip地址的网卡,于是造成冲突.
		4,回环网卡,其实就是发送和接收是两个模块相对独立,发送的请求,直接从发送端发到接收端,而不用经过外部局域网路由器
			也可以理解外,客户端 连接的 远程服务 端就是自身pc运行的与客户端程序相互独立的一个 服务端程序
			也可以理解外,服务端 连接的 远程客户 端就是自身pc运行的与服务端程序相互独立的一个 客户端程序
			至于两个程序使用相同ip地址的理解为,使用同一个网卡!!
			两个程序先后使用同一个网卡的发送功能/接收功能,所以不冲突.

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



#define portnum		4444		//自定义的一个端口. 注意测试过程中不能选低于1024的端口号,因为都是正式的国际规定的专用端口


int main (void)
{
	int sockfd;
	struct sockaddr_in server_addr;			//存储ipv4网络地址数据的结构,而struct sockaddr是存储通用网络地址数据的结构
	struct sockaddr_in client_addr;	
	char wbuf[128];
	int nbyte;					//一次接收函数实际接收的字节数
	socklen_t sockaddr_len = sizeof(struct sockaddr);
	int ret;					//connect状态


	//先创建一个套接字
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1 )	//执行tcp协议的套接字,protocol可默(认为0
	{							//生成的套接字文件可看作是一个打开了的特殊的文件,所以成功时,返回文件描述符
		printf("create socket fair\n");			//生成的socker文件,会自动绑定一个自动分配的地址
		exit(1);
	}

	bzero(&server_addr,sizeof(struct sockaddr_in));				//清零server_addr这个结构数据变量
	//填写服务器地址信息
	server_addr.sin_family = AF_INET;		//填写类型
	server_addr.sin_port = htons(portnum);		//填写端口,转为网络字节序才可以赋值使用
	//server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");	//填写地址,这个字符转成整形时自动转为网络字节序才可以赋值使用
	//server_addr.sin_addr.s_addr = inet_addr("192.168.0.107");	//填写地址,inet_addr:字符串转成整形,并自动转为网络字节序,
	//server_addr.sin_addr.s_addr = inet_addr("192.168.0.102");	//填写地址,inet_addr:字符串转成整形,并自动转为网络字节序,
	server_addr.sin_addr.s_addr = inet_addr("192.168.0.116");	//填写地址,inet_addr:字符串转成整形,并自动转为网络字节序,
	//server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 	//填写地址,相当于 inet_addr("127.0.0.1")
	//连接服务器
	ret = connect(sockfd,(struct sockaddr *)(&server_addr),sockaddr_len);
	perror("connect");	//显示链接状态
	if( ret == -1 )
	{
		printf("connect fair!!\n");
		exit(1);
	}

	while(1)
	{
		//输入传输数据
		printf("enter:\n");
		fgets(wbuf,128,stdin);			//从标准输入设备(已映射到键盘)输入数据 ,回车符结束输入
	
		//发送数据
		send(sockfd,wbuf,strlen(wbuf),0);	//使用strlen算字符串长度,不会算入最后的结束符.
	}
	//关闭整个网络连接服务`,其实就是,关闭用来监听的套接字
	close(sockfd);

	return 0;
}






































































































