#define GPJ2CON *((volatile unsigned long*)0xE0200280)
#define GPJ2DAT *((volatile unsigned long*)0xE0200284)

void led_init()
{   
    GPJ2CON = 0x1;
}

void led_off()
{
    GPJ2DAT = 0x1;
}

void led_on()
{
    GPJ2DAT = 0x0;
}
