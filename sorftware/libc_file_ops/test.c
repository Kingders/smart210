//善用man 查看

#include <stdio.h> 	//fopen(),fclose(),fread(),fwrite(),fseek()...

#define pathA	"./kl"	//当前工作目录下的文件路径

int main (void)
{
	FILE *fp;	//一个流空间指针
	int ret;	//关闭操作返回状态, 
	int sek;	//deek定位返回状态:成功返0,失败返-1

	size_t	rsize;	
	char rbuffer[20];	//注意不能用字符串指针!!只能用字符数组

	size_t	wsize;	
	char *wbuffer;		//


	/**** fopen ****/	//严格上看是,把一个文件的数据 的局部 暂存到一个新建的流空间里,
	//fp = fopen(pathA,"w+");	//W+ 开放读写权限,文件不存在就创建,如果文件存在就先清空文件内容,第二个参数是字符串方式 
	fp = fopen(pathA,"r+");	//r+ 开放读写权限
	if (fp == NULL)		//NULL 一定要大写
		printf("fair\n\r");
	/* 流空间里的操作指针: hello                   */
	/*                    ^                       */
	/*                    i love you, oh my god!! */
	

	/**** fread ****/
	rsize = fread(rbuffer,2,5,fp);	//从流空间开始处读出 5个 数据块,数据块的大小为 2 字节, 共读出10字节放到rbuffer里
					//读成功后,返回共读出的数据块数,这里应该为 5
	rbuffer[10] = '\0';		//给字符串添加结束符NULL	
	if(rsize == 5)		
		printf("%s\n\r",rbuffer);
	/* 流空间里的操作指针: hello                   */
	/*                    i love you, oh my god!! */
	/*                        ^                   */


	/**** fwrite ****/
	wbuffer = "open_the_door";
	wsize = fwrite(wbuffer,2,5,fp);//从wbuffer写入 5个 数据块,数据块的大小为 2 字节, 共10字节放到流空间文件操作指针目前指向位置
	if(wsize != 5)		
		printf("fair_write\n\r");
	/* 流空间里的操作指针: hello                   */
	/*                    i loopen_the_d my god!! */
	/*                                  ^         */
	/* 此刻硬盘文件kl内容: hello                   */
	/*                    i love you, oh my god!! */


	/**** fseek (write)****/
	//这里更深入ｃ库的文件编辑模型，
	//假如一个文件 300 字节,一个流空间100个字节,用c库规则打开时,会把文件的前100字节填入流空间,
	//这时两个操作指针,流空间操作指针A,和文件操作指针B,两者映射关系:B指向第n个字节,A指向第n个字节
	//当B值大于100(小于200),导致A值将大于100时,于是c库把流空间100字节刷回文件,并把第101-200的字节刷入流空间,
	//这时 B指向第n个字节,A指向第 n-100 个字节,如此类推
	//fseek()是通过操作 文件操作指针 实现 定位的,当然,会为此自动刷写对应的内容,和转化出对应的 流空间操作指针
	sek = fseek(fp,2,SEEK_CUR);	//从当前的 文件操作指针位置 后移2个字节得新位置, 再映射到流空间操作指针.
					//由于测试文件并不大,流空间可完全装载,所以 流空间操作指针==文件操作指针
	if (sek == -1)			
		printf("fair_seek\n\r");
	/* 流空间里的操作指针: hello                   */
	/*                    i loopen_the_d my god!! */
	/*                                    ^       */
	wbuffer = "_ok";
	wsize = fwrite(wbuffer,1,3,fp);
	if(wsize != 3)		
		printf("fair_write2\n\r");
	/* 流空间里的操作指针: hello                   */
	/*                    i loopen_the_d m_okod!! */
	/*                                       ^    */	
	/* 此刻硬盘文件kl内容: hello                   */
	/*                    i love you, oh my god!! */




	getchar();	//文件结束前先获取一个任意值,目的观察:写入操作先把内容暂时写入流空间,
			//若因为空间溢出,或者要关闭文件,才把修改的内容刷入目标文件
	/* 此刻硬盘文件kl内容: hello                   */
	/*                    i love you, oh my god!! */
	ret = fclose(fp);
	/* 此刻硬盘文件kl内容: hello                   */
	/*                    i loopen_the_d m_okod!! */
	if(ret != EOF)
		return 0;
}
