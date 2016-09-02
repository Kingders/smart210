/* add by zjh */

#ifndef __asm_arm_arch_dram_h_
#define __asm_arm_arch_dram_h_

#ifndef __assembly__

struct s5pv210_lcd {
	unsigned int	vidcon0;
	unsigned int	vidcon1;
	unsigned int	vidcon2;
	unsigned int	vidcon3;
	unsigned int	vidtcon0;
	unsigned int	vidtcon1;
	unsigned int	vidtcon2;
	unsigned int	vidtcon3;
	unsigned int	wincon0;
	unsigned int	wincon1;
	unsigned int	wincon2;
	unsigned int	wincon3;
	unsigned int	wincon4;
	unsigned int	shadowcon;
	unsigned char	res0[0x8];
	unsigned int	vidosd0a;
	unsigned int	vidosd0b;
	unsigned int	vidosd0c;
	unsigned char	res1[0x4];
	unsigned int	vidosd1a;
	unsigned int	vidosd1b;
	unsigned int	vidosd1c;
	unsigned int	vidosd1d;
	unsigned int	vidosd2a;
	unsigned int	vidosd2b;
	unsigned int	vidosd2c;
	unsigned int	vidosd2d;
	unsigned int	vidosd3a;
	unsigned int	vidosd3b;
	unsigned int	vidosd3c;
	unsigned char	res2[0x4];
	unsigned int	vidosd4a;
	unsigned int	vidosd4b;
	unsigned int	vidosd4c;
	unsigned char	res3[0x14];
	unsigned int	vidw00add0b0;
	unsigned int	vidw00add0b1;
	unsigned int	vidw01add0b0;
	unsigned int	vidw01add0b1;
	unsigned int	vidw02add0b0;
	unsigned int	vidw02add0b1;
	unsigned int	vidw03add0b0;
	unsigned int	vidw03add0b1;
	unsigned int	vidw04add0b0;
	unsigned int	vidw04add0b1;
	unsigned char	res4[0x8];
	unsigned int	vidw00add1b0;
	unsigned int	vidw00add1b1;
	unsigned int	vidw01add1b0;
	unsigned int	vidw01add1b1;
	unsigned int	vidw02add1b0;
	unsigned int	vidw02add1b1;
	unsigned int	vidw03add1b0;
	unsigned int	vidw03add1b1;
	unsigned int	vidw04add1b0;
	unsigned int	vidw04add1b1;
};

#endif

#endif
