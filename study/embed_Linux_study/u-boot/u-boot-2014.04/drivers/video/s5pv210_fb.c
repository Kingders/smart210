/*
** add by zjh
*/

#include <common.h>
#include <lcd.h>
#include <asm/arch/lcd_reg.h>

#define GPD0CON			*((volatile u32 *)0xE02000A0)
#define GPD0DAT			*((volatile u32 *)0xE02000A4)

#define GPF0CON			*((volatile u32 *)0xE0200120)
#define GPF1CON			*((volatile u32 *)0xE0200140)
#define GPF2CON			*((volatile u32 *)0xE0200160)
#define GPF3CON			*((volatile u32 *)0xE0200180)

#define DISPLAY_CONTROL	*((volatile u32 *)0xE0107008)

#define VBPD	23
#define VFPD	22
#define VSPW	2

#define HBPD	46
#define HFPD	210
#define HSPW	2

vidinfo_t panel_info = {
	.vl_col = 800,
	.vl_row = 480,
	.vl_bpix = LCD_COLOR24,
};

u32 calc_fbsize(void)
{
	int line_length = (panel_info.vl_col * NBITS(panel_info.vl_bpix)) / 8;
	return line_length * panel_info.vl_row;
}

void lcd_ctrl_init(void *lcdbase)
{
	u32 cfg = 0;
	
	struct s5pv210_lcd *const lcd = (struct s5pv210_lcd *)samsung_get_base_lcd();
	
	GPF0CON  = 0x22222222;						/* LCD_VD[3:0] LCD_VCLK LCD_VDEN LCD_VSYNC LCD_HSYNC */
	GPF1CON  = 0x22222222;						/* LCD_VD[11:4] */
	GPF2CON	 = 0x22222222;						/* LCD_VD[19:12] */
	GPF3CON	 = 0x00002222;						/* LCD_VD[23:20] */
	GPD0CON  = (GPD0CON & ~0xF) | (0x1 << 0);	/* LCD PWR(backlight) */
	
	DISPLAY_CONTROL = (2 << 0);					/* Display path selection:RGB=FIMD I80=FIMD ITU=FIMD */

	/* Configures video output format and displays enable/disable */
	cfg  = 	(5 << 6) |				/* CLKVAL = 4, HCLK(166MHz) / (5 + 1) = 27MHz */
			(1 << 4) |				/* Selects the clock source as divide using CLKVAL_F */
			(0 << 2);				/* Selects the video clock source:HCLK=166MHz */
	writel(cfg, &lcd->vidcon0);
	
	/* Specifies RGB I/F control signal */
	cfg  = 	(0 << 7) |				/* Video data is fetched at VCLK falling edge */
			(1 << 6) |				/* Inverted HSYNC pulse polarity */
			(1 << 5) |				/* Inverted VSYNC pulse polarity */
			(0 << 4);				/* Normal VDEN signal polarity ?????*/
	writel(cfg, &lcd->vidcon1);
	
	/* Specifies output data format control */
	cfg  = 	(0b000 << 19) |			/* RGB interface output order(Even line, line #2,4,6,8):RGB */
			(0b000 << 16);			/* RGB interface output order(Odd line, line #1,3,5,7):RGB */
	writel(cfg, &lcd->vidcon2);
	
	/* Configures video output timing and determines the size of display */
	cfg =	(VBPD << 16) |			/* VBPD */
			(VFPD << 8) |			/* VFPD */
			(VSPW << 0);			/* VSPW */
	writel(cfg, &lcd->vidtcon0);
				
	cfg =	(HBPD << 16) |			/* HBPD */
			(HFPD << 8) |			/* HFPD */
			(HSPW << 0);			/* HSPW */
	writel(cfg, &lcd->vidtcon1);

	cfg = 	((panel_info.vl_row - 1) << 11) |		/* vertical size of display(LINEVAL + 1) */
			(panel_info.vl_col - 1);				/* horizontal size of display(HOZVAL + 1) */
	writel(cfg, &lcd->vidtcon2);
	
	writel(0b1 << 31, &lcd->vidtcon3);	/* Enables VSYNC Signal Output */

	cfg  =	(0 << 30) |				/* BUFSEL_H = 0 */
			(0 << 20) |				/* BUFSEL_L = 0, BUFSEL=0b00(Selects the Buffer set 0) */
			(1 << 15) |				/* the Word swap Enable */
			(0xB << 2);				/* Unpacked 24 bpp ( non-palletized R:8-G:8-B:8 ) */
	writel(cfg, &lcd->wincon0);
	
	/* Specifies window position setting */
	writel((0 << 11) | 0, &lcd->vidosd0a);	/* LeftTop */
	
	cfg = 	((panel_info.vl_col - 1) << 11) |	/* RightBotX */
			(panel_info.vl_row - 1);			/* RightBotY */
	writel(cfg, &lcd->vidosd0b);
	
	/* Specifies the Window Size:Height * Width (Number of Word) */
	writel(calc_fbsize() / 4, &lcd->vidosd0c);
	
	/* the start address for Video frame buffer */
	writel((u32)lcdbase, &lcd->vidw00add0b0);
	writel((u32)lcdbase + calc_fbsize(), &lcd->vidw00add1b0);

	writel(0x1, &lcd->shadowcon);	/* Enables Channel 0 */
	
	GPD0DAT |= 1;					/* backlight */
}

void lcd_enable(void)
{
	struct s5pv210_lcd *const lcd = (struct s5pv210_lcd *)samsung_get_base_lcd();
	writel(readl(&lcd->vidcon0) | 0x3, &lcd->vidcon0);
	writel(readl(&lcd->wincon0) | 0x1, &lcd->wincon0);
}

/*
** 将NAND FLASH的log分区的数据读取到环境变量
** splashimage指定的内存地址
*/
int splash_screen_prepare(void)
{
	char *s = NULL;
	char cmd[100];
	
	if ((s = getenv("splashimage")) == NULL)
	{
		printf("Not set environable: splashimage\n");
		return 1;
	}
	
	sprintf(cmd, "nand read %s log", s);
	return run_command_list(cmd, -1, 0);
}
