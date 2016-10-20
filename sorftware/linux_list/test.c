/*************************************************************

    #ifndef _LIST_H_  
    #define _LIST_H_  
      
    /********************************************************** 
    功能: 计算MEMBER成员在TYPE结构体中的偏移量 
    **********************************************************/  
    #define offsetof(TYPE, MEMBER)  (unsigned long)(&(((TYPE*)0)->MEMBER))  
      
      
    /********************************************************** 
    功能: 计算链表元素的起始地址 
    输入:  
        ptr：    type结构体中的链表指针 
        type:   结构体类型 
        member: 链表成员名称 
    **********************************************************/  
    #define container_of(ptr, type, member) (type *)((char*)(ptr) - offsetof(type, member))  
      
    #define LIST_HEAD_INIT(name)    {&(name), &(name)}  
      
    struct list  
    {  
        struct list *prev, *next;  
    };  
      
    static inline void list_init(struct list *list)  
    {  
        list->next = list;  
        list->prev = list;  
    }  
      
    static inline int list_empty(struct list *list)  
    {  
        return list->next == list;  
    }  
      
    // 将new_link插入到link之前  
    static inline void list_insert(struct list *link, struct list *new_link)  
    {  
        new_link->prev        = link->prev;  
        new_link->next        = link;  
        new_link->prev->next = new_link;  
        new_link->next->prev = new_link;  
    }  
      
    /********************************************************** 
    功能: 将new_link节点附加到list链表中 
    **********************************************************/  
    static inline void list_append(struct list *list, struct list *new_link)  
    {  
        list_insert(list, new_link);  
    }  
      
    /********************************************************** 
        功能: 从链表中移除节点 
    **********************************************************/  
    static inline void list_remove(struct list *link)  
    {  
        link->prev->next = link->next;  
        link->next->prev = link->prev;  
    }  
      
    /********************************************************** 
    获取link节点对应的结构体变量地址 
    link:   链表节点指针 
    type:   结构体类型名 
    member: 结构体成员变量名 
    **********************************************************/  
    #define list_entry(link, type, member)  container_of(link, type, member)  
      
      
    /********************************************************** 
    获取链表头节点对应的结构体变量地址 
    list:   链表头指针 
    type:   结构体类型名 
    member: 结构体成员变量名 
    Note: 
    链表头节点实际为链表头的下一个节点,链表头未使用，相当于哨兵 
    **********************************************************/  
    #define list_head(list, type, member) list_entry((list)->next, type, member)  
      
    /********************************************************** 
    获取链表尾节点对应的结构体变量地址 
    list:   链表头指针 
    type:   结构体类型名 
    member: 结构体成员变量名 
    **********************************************************/  
    #define list_tail(list, type, member) list_entry((list)->prev, type, member)  
      
    /********************************************************** 
    返回链表下一个节点对应的结构体指针 
    elm:    结构体变量指针 
    type:   结构体类型名 
    member: 结构体成员变量名(链表变量名) 
    **********************************************************/  
    #define list_next(elm,type,member) list_entry((elm)->member.next, type, member)  
      
    /********************************************************** 
    遍历链表所有节点对应的结构体 
    pos : 结构体指针 
    type : 结构体类型名 
    list : 链表头指针 
    member : 结构体成员变量名(链表变量名) 
    Note : 链表头未使用，因此遍历结束后，pos指向的不是有效的结构体地址 
    **********************************************************/  
    #define list_for_each_entry(pos, type, list, member)    \  
    for (pos = list_head(list, type, member);               \  
        &pos->member != (list);                              \  
        pos = list_next(pos, type, member))  
      
      
    /********************************************************** 
    example function 
    **********************************************************/  
    void list_example(void);  
    #endif  

linux链表系统的指针域结构:
struct list_head{
	struct list_head *next,*prev;
}

通过使用加载一个内核模块来测试链表



**************************************************************/


//linux模块加载使用的头文件
#include <linux/module.h>
#include <linux/init.h>
//linux链表先关头文件
#include <linux/list.h>
#include <>
#include <>

//定义一个链成员
struct score{		//链成员,一个学生的成绩表
	//内容域
	int num;	//学号
	int math;	//数学成绩
	int english;	//英语成绩表
	//指针域
	struct list_head list_pointer;
};

//声明参数
struct list_head header;	//作为链表头的一个指针域参数,某意义上也代表这一条链表
struct score tom,kate,jack;	//3个链成员,3张学生成绩表
struct list_head *pos;		//一个指向指针域的的指针,知道某指针域的位置,也就可以找到包含这个指针域的链成员
struct score *student		//一个指向链成员的的指针,

static int list_init(void)
{
	INIT_LIST_HEAD(&header);	//初始化一个链表,

	//tom的成绩表
	tom.num =1;
	tom.math = 89;
	tom.english = 56;
	//kate的成绩表
	kate.num =2;
	kate.math = 20;
	kate.english = 74;
	//jack的成绩表
	jack.num =3;
	jack.math = 99;
	jack.english = 46;

	//把三张成绩单链成员加入链表
	list_add_tail(&(tom.list),&header);
	list_add_tail(&(kate.list),&header);
	list_add_tail(&(jack.list),&header);
	
	//遍历链表,把所有人的成绩表内容都显示
	list_for_each(pos,&header)	//遍历操作,其实是一个for循环头的宏定义,pos用来存取当前遍历到的指针域的位置,header指示了遍历哪条链表
	{
		student = list_entry(pos,struct score, list_pointer);	//取出当前遍历到的指针域 对应的链成员的位置,并放入student
									//参数struct score,表示返回的链成员结构,
									//参数list_pointer,表示指针域 在struct score链成员里的位置名称
									//由于指针域放在链结构里,
									//参数list_pointer,参数struct score,
									//可以算出指针域距离链成员始地址的偏移,
									//当前指针域的位置减去偏移,就得到指针域所在链成员的始地址
		printk("NO:%d,math%d,english%d\n",student->num,student->math,student->english);
	}
	return 0;
}

static int list_exit(void)
{
	//从链表里删除两个链成员
	list_del(&(tom.list));
	list_del(&(jack.list));
	//删除链表的操作未测试,
	return 0;
}

MODULE_LICENSE("GPL");

module_init(list_init);
module_exit(list_exit);
