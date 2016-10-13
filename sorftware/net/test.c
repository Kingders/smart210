



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

//地址转换 
in_addr_t inet_addr(const char *cp);	//把一个字符串地址例"192.168.1.1"转成数型,而且也自动转成网络字节序
char* inet_ntoa(struct in_addr);	//把一个整数地址转成字符串型


listen()				//让socket文件准备好接受客户端访问.

accept()				//等待客户端连接,执行时,如果没有客户端连接时,阻塞服务器程序,等待直到有访问.
					//成功等到后,返回一个新的socket套接字文件描述符.作为与这个客户端的专门通道.


connect					//客户端的专门函数,连接服务端







