/*interrupt registes*/
#define EXT_INT_2_CON       *((volatile unsigned int *)0xE0200E08)   
#define EXT_INT_2_MASK      *((volatile unsigned int *)0xE0200F08)       
#define VIC0INTENABLE       *((volatile unsigned int *)0xF2000010)   
#define key_VICADDR         *((volatile unsigned int *)0xF2000140)     
#define VIC0ADDRESS         *((volatile unsigned int *)0xF2000F00)   
#define EXT_INT_2_PEND      *((volatile unsigned int *)0xE0200F48)  

void key_isr()
{
    volatile unsigned int key_code;
    
    __asm__( 				//保存环境
    					//中断程序中,改变的只是pc指针值,在其他寄存器被中断程序改变前先要包存起来
    "sub lr, lr, #4\n"  		//计算ldr
    "stmfd sp!, {r0-r12, lr}\n"       	//把r1~r12,以及lr 压入栈
    : 
    : 
   );

 
    key_code = EXT_INT_2_PEND & 0b11; 
	
    switch(key_code) 
    {
        case 1: //K1
            led_on();
            break;
		
        case 2: //K2
            led_off();
            break;
		
        default:
            break;
    }
	
    /* 清除中断 */    
    EXT_INT_2_PEND =  ~0x0;  		//清空相当于对应flags
    VIC0ADDRESS = 0;			//中断函数发生时,中断函数的地址也会保存到这里来!!,结束时记得清理掉		
   
    __asm__( 				//回复环境
    "ldmfd sp!, {r0-r12, pc}^ \n"       //把栈里的东西取回来
    : 
    : 
  );
}



void init_irq()
{
    EXT_INT_2_CON = 0b010 | (0b010<<4);    /*EINT16,17下降沿触发中断,EINT[16]~[31]都对应vic16这个寄存器放置的中断处理函数地址*/
    EXT_INT_2_MASK = 0;                   /* 取消屏蔽外部中断,开启中断*/  
 
    VIC0INTENABLE = 0b1<<16;              /*开放vic[16]这个中断地址*/ 
    key_VICADDR = (int)key_isr;           /*由于key1~key4属于同一中断源，所以只需设置一个地址*/
    
    __asm__( 
    /*开中断*/  
    "mrs r0,cpsr\n"
    "bic r0, r0, #0x80\n"
    "msr cpsr_c, r0\n"            
    : 
    : 
  );
}
