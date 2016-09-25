#include <linux/platform_device.h>  
#include <linux/clk.h>  
#include <linux/err.h>  
#include <linux/io.h>  
#include <linux/delay.h>  
#include <mach/regs-clock.h>  
#include <mach/gpio.h>  
#include <mach/regs-sys.h>  
#include <plat/regs-usb-hsotg-phy.h>  
#include <plat/usb-phy.h>  
#include <plat/clock.h>  
#include <plat/gpio-cfg.h>  
      
int s5p_usb_phy_init(struct platform_device *pdev, int type)  
{
	int err;
	struct clk *otg_clk;

	if (type != S5P_USB_PHY_HOST)
		return -EINVAL;

	otg_clk = clk_get(&pdev->dev, "otg");
	if (IS_ERR(otg_clk)) {
		dev_err(&pdev->dev, "Failed to get otg clock\n");
		return PTR_ERR(otg_clk);  
	}

	err = clk_enable(otg_clk);
	if (err){
		clk_put(otg_clk);
		return err;
	}

        if (readl(S5PV210_USB_PHY_CON) & (0x1<<1)) {
		clk_disable(otg_clk);
		clk_put(otg_clk);
		return 0;
	}

	__raw_writel(__raw_readl(S5PV210_USB_PHY_CON) | (0x1<<1),
		S5PV210_USB_PHY_CON);
	__raw_writel((__raw_readl(S3C_PHYPWR)
			& ~(0x1<<7) & ~(0x1<<6)) | (0x1<<8) | (0x1<<5) | (0x1<<4),
		S3C_PHYPWR);
	__raw_writel((__raw_readl(S3C_PHYCLK) & ~(0x1<<7)) | (0x3<<0),
		S3C_PHYCLK);
	__raw_writel((__raw_readl(S3C_RSTCON)) | (0x1<<4) | (0x1<<3),
		S3C_RSTCON);
	__raw_writel(__raw_readl(S3C_RSTCON) & ~(0x1<<4) & ~(0x1<<3),
		S3C_RSTCON);
	/* "at least 10uS" for PHY reset elsewhere, 20 not enough here... */  
	udelay(50);

	clk_disable(otg_clk);
	clk_put(otg_clk);

	return 0;
}  
      
      
int s5p_usb_phy_exit(struct platform_device *pdev, int type)  
{
	if (type != S5P_USB_PHY_HOST)
		return -EINVAL;

	__raw_writel(__raw_readl(S3C_PHYPWR) | (0x1<<7)|(0x1<<6),
		S3C_PHYPWR);
	__raw_writel(__raw_readl(S5PV210_USB_PHY_CON) & ~(1<<1),
		S5PV210_USB_PHY_CON);

	return 0;
}  
