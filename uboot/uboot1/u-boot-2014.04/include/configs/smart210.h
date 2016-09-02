/*
 * (C) Copyright 2009 Samsung Electronics
 * Minkyu Kang <mk7.kang@samsung.com>
 * HeungJun Kim <riverful.kim@samsung.com>
 * Inki Dae <inki.dae@samsung.com>
 *
 * Configuation settings for the SAMSUNG SMDKC100 board.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_SAMSUNG		1	/* in a SAMSUNG core */
#define CONFIG_S5P		1	/* which is in a S5P Family */
#define CONFIG_S5PC100		1	/* which is in a S5PC100 */
#define CONFIG_SMDKC100		1	/* working with SMDKC100 */

#include <asm/arch/cpu.h>		/* get chip and board defs */

#define CONFIG_ARCH_CPU_INIT

#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_DISPLAY_BOARDINFO

/* input clock of PLL: SMDKC100 has 12MHz input clock */
#define CONFIG_SYS_CLK_FREQ		12000000
/* input clock of PLL: SMDKV210 has 24MHz input clock(add by zjh) */
#define CONFIG_SYS_CLK_FREQ_V210	24000000

/* DRAM Base */
#define CONFIG_SYS_SDRAM_BASE		0x20000000	/* modied by zjh */

/* Text Base */
#define CONFIG_SYS_TEXT_BASE		0x20000000	/* modied by zjh */

#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_CMDLINE_EDITING

/*
 * Size of malloc() pool
 * 1MB = 0x100000, 0x100000 = 1024 * 1024
 */
#define CONFIG_SYS_MALLOC_LEN		(CONFIG_ENV_SIZE + (1 << 20))
/*
 * select serial console configuration
 */
#define CONFIG_SERIAL0			1	/* use SERIAL 0 on SMDKC100 */

/* PWM */
#define CONFIG_PWM			1

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_BAUDRATE			115200

/***********************************************************
 * Command definition
 ***********************************************************/
#include <config_cmd_default.h>

#define CONFIG_CMDLINE_EDITING	/* add by zjh */
#define CONFIG_AUTO_COMPLETE	/* add by zjh */
/* add by zjh, for bootmenu */
#define CONFIG_CMD_BOOTMENU
#define CONFIG_MENU
#define CONFIG_AUTOBOOT_KEYED
#define CONFIG_MENU_SHOW

#undef CONFIG_CMD_FLASH
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_NAND

#define CONFIG_CMD_CACHE
#define CONFIG_CMD_REGINFO
/* masked by zjh */
#if 0
/* add by zjh */
#define CONFIG_CMD_ONENAND
#endif
#define CONFIG_CMD_NAND
#define CONFIG_CMD_ELF
#define CONFIG_CMD_FAT
#define CONFIG_CMD_MTDPARTS

#define CONFIG_BOOTDELAY	3

#define CONFIG_ZERO_BOOTDELAY_CHECK

/* add by zjh */
#define CONFIG_SYS_MAX_NAND_DEVICE 	1
#define CONFIG_SYS_NAND_BASE		0xB0E00000
#define CONFIG_NAND_S5PV210
#define CONFIG_S5PV210_NAND_HWECC
#define	CONFIG_SYS_NAND_ECCSIZE		512
#define CONFIG_SYS_NAND_ECCBYTES	13
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS

/* modied by zjh */
#define MTDIDS_DEFAULT		"nand0=s5p-nand"
#define MTDPARTS_DEFAULT	"mtdparts=s5p-nand:256k(bootloader)"\
				",128k@0x40000(params)"\
				",2m@0x60000(log)"\
				",3m@0x260000(kernel)"\
				",-(rootfs)"

#define NORMAL_MTDPARTS_DEFAULT MTDPARTS_DEFAULT

#define CONFIG_BOOTCOMMAND	"run ubifsboot"

#define CONFIG_RAMDISK_BOOT	"root=/dev/ram0 rw rootfstype=ext2" \
				" console=ttySAC0,115200n8" \
				" mem=128M"

#define CONFIG_COMMON_BOOT	"console=ttySAC0,115200n8" \
				" mem=128M " \
				" " MTDPARTS_DEFAULT

#define CONFIG_BOOTARGS	"root=/dev/mtdblock5 ubi.mtd=4" \
			" rootfstype=cramfs " CONFIG_COMMON_BOOT

#define CONFIG_UPDATEB	"updateb=onenand erase 0x0 0x40000;" \
			" onenand write 0x32008000 0x0 0x40000\0"



#define CONFIG_ENV_OVERWRITE
/* modied by zjh */
#define CONFIG_EXTRA_ENV_SETTINGS					\
	"splashimage=0x23000000" \


/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_HUSH_PARSER		/* use "hush" command parser	*/
#define CONFIG_SYS_PROMPT		"SMART210 # "	/* modied by zjh */
#define CONFIG_SYS_CBSIZE	256	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE	384	/* Print Buffer Size */
#define CONFIG_SYS_MAXARGS	16	/* max number of command args */
/* Boot Argument Buffer Size */
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
/* memtest works on */
#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_SDRAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_SDRAM_BASE + 0x5e00000)
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_SDRAM_BASE

/* SMART210 has 1 banks of DRAM, we use only one in U-Boot */
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1		CONFIG_SYS_SDRAM_BASE	/* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	(512 << 20)	/* 0x20000000, 512 MB Bank #1 (modied by zjh) */

#define CONFIG_SYS_MONITOR_BASE	0x00000000

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CONFIG_SYS_NO_FLASH		1

#define CONFIG_SYS_MONITOR_LEN		(256 << 10)	/* 256 KiB */
#define CONFIG_IDENT_STRING		" for SMART210"	/* modied by zjh */

#if !defined(CONFIG_NAND_SPL) && (CONFIG_SYS_TEXT_BASE >= 0xc0000000)
#define CONFIG_ENABLE_MMU
#endif

#ifdef CONFIG_ENABLE_MMU
#define CONFIG_SYS_MAPPED_RAM_BASE	0xc0000000
#else
#define CONFIG_SYS_MAPPED_RAM_BASE	CONFIG_SYS_SDRAM_BASE
#endif

/*-----------------------------------------------------------------------
 * Boot configuration
 */
/* masked by zjh */
#if 0
#define CONFIG_ENV_IS_IN_ONENAND	1
#endif
/* add by zjh */
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_SIZE			(128 << 10)	/* 128KiB, 0x20000 */
#define CONFIG_ENV_ADDR			(256 << 10)	/* 256KiB, 0x40000 */
#define CONFIG_ENV_OFFSET		(256 << 10)	/* 256KiB, 0x40000 */

#define CONFIG_USE_ONENAND_BOARD_INIT
#define CONFIG_SAMSUNG_ONENAND		1
#define CONFIG_SYS_ONENAND_BASE		0xE7100000

#define CONFIG_DOS_PARTITION		1

#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_SDRAM_BASE + PHYS_SDRAM_1_SIZE)	/* modied by zjh */

/*
 * Ethernet Contoller driver
 */
#ifdef CONFIG_CMD_NET
/* masked by zjh */
#if 0
#define CONFIG_SMC911X         1       /* we have a SMC9115 on-board   */
#define CONFIG_SMC911X_16_BIT  1       /* SMC911X_16_BIT Mode          */
#define CONFIG_SMC911X_BASE    0x98800300      /* SMC911X Drive Base   */
#endif
/* modied by zjh */
#define CONFIG_ENV_SROM_BANK   1       /* Select SROM Bank-1 for Ethernet*/
/* add by zjh */
#define CONFIG_DRIVER_DM9000
#define CONFIG_DM9000_NO_SROM
#define CONFIG_DM9000_BASE		0x88000000
#define DM9000_IO				(CONFIG_DM9000_BASE)
#define DM9000_DATA				(CONFIG_DM9000_BASE + 0x4)
#define CONFIG_CMD_PING
#define CONFIG_IPADDR			192.168.2.120
#define CONFIG_SERVERIP			192.168.2.227
#define CONFIG_ETHADDR			1A:2A:3A:4A:5A:6A
#endif /* CONFIG_CMD_NET */

#define CONFIG_SPL /* add by zjh */

/* add by zjh */
#if 0
#define CONFIG_LCD
#define CONFIG_S5PV210_LCD
#define LCD_BPP					LCD_COLOR24
#define CONFIG_SPLASH_SCREEN
#define CONFIG_CMD_BMP
#define CONFIG_BMP_32BPP
#endif
#endif	/* __CONFIG_H */
