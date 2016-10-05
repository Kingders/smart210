/**********************************************
* man 2 open	open函数在man第二章节 但直接man open 会显示第一章节的openenv
* man read	read函数在man第二章节
*
*
***********************************************/


#include <stdio.h>
#include <fcntl.h>	//open()
#include <unistd.h>	//close() read() write() lseek() dup()
#include <sys/types.h>	//lseek()

#define pathA  "./kl"	// 在当前工作目录的文件的路径

int main (void)
{
	int fd;
	int rsize;		//读出了多少个字符
	char rbuff[10];		
	
	int wsize;		//写了多少个字符
	char *wbuff;		//写函数要求用字符串指针,而不是字符数组.

	int hoffset;		//从文件开始数起第hoffset个字节

	int newfd;

	/**** open ****/
	//fd = open(pathA,O_CREAT,S_IRWXU);	//打开文件,返回fd,O_CREAT新建文件方式, S_IRWXU文件开放全权限
	fd = open(pathA,O_RDWR);	//打开存在的文件
	if (fd<0)
		printf("fair\n\r");
	/*pointer:  linux is me*/
	/*          ^         */		


	/**** read ****/
	rsize = read(fd,rbuff,4);	//buff  一定要是buff[]字符数组,不能是*buff字符指针 ,读出4个字符,文件操作指针来到第五个字符的位置,
	//buff[4] = NULL ;		//buff是字符数组类型,不能这样赋结束符!!
	rbuff[4] = '\0';		//要这样赋值, 用单引号传递一个字符,双引号传递一串字符串, '\0'表示 null
	if (rsize > 0)
		printf("%s\n\r",rbuff);
	/*pointer:  linux is me*/
	/*              ^      */


	/**** write ****/
	wbuff = "\n i am king ";	//先转行,然后空格,然后输入i am king,再输入一个空格,这串字符共12个字节	
	wsize = write(fd,wbuff, 12);	//这里不管第五个字符后有没有内容,从第五个字符的位置开始直接复写12个字节,
					//然后文件操作指针(当前位置)指向最后一个空格字符的后一个字符.
	/*pointer:  linu         */
	/*           i am king   */
	/*                     ^ */


#if 1
	/**** lseek (write) ****/
	//hoffset = lseek(fd,4,SEEK_SET);	//文件操作指针来到,从头开始的第1个字符位置,再转到当前位置的后数4个字符位置
						//即指向了转行符'\0'
	hoffset = lseek(fd,-2,SEEK_CUR);	//文件操作指针来到,从当前位置的前数2个字符位置,
	/*pointer:  linu         */		//即指向 i am king 中的 g 字符位置
	/*           i am king   */
	/*                   ^   */
	wbuff = "zzzlllkkk";		//9个字符的字符串
	wsize = write(fd,wbuff, 9);	//在文件操作指针当前位置开始写入数据,注意写入不多于9个字符,因为字符串只有9个,多于9个会破坏文件
	/*pointer:  linu         	*/
	/*           i am kinzzzlllkkk  */
	/*                            ^ */

#endif
	/**** dup ****/			//复制一个文件描述符

	newfd = dup (fd);
	/*new_pointer:  linu         	    */
	/*               i am kinzzzlllkkk  */
	/*                                ^ */
	hoffset = lseek(newfd,-9,SEEK_CUR);	//操作新文件描述符
						//注意,新旧的文件描述符,共用文件操作指针
	/*new_pointer:  linu         	    */
	/*               i am kinzzzlllkkk  */
	/*                       ^          */
	/*pointer:  linu         	    */
	/*               i am kinzzzlllkkk  */
	/*                       ^          */
	wbuff = "_ok";		//3个字符的字符串
	wsize = write(fd,wbuff, 3);	//通过旧文件描述符写入 
	/*pointer:  linu         	 */
	/*           i am kin_oklllkkk   */
	/*                      ^        */
	/*new_pointer:  linu         	    */
	/*               i am kin_oklllkkk  */
	/*                          ^       */
	close(newfd);	//关闭新文件描述符,仍可以继续通过就文件描述符操作文件

	close(fd);	//关闭文件

	return 0;
}
