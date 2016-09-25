#ifndef __SETUP_USB_PHY_H
#define __SETUP_USB_PHY_H

int s5p_usb_phy_init(struct platform_device *pdev, int type);
int s5p_usb_phy_exit(struct platform_device *pdev, int type);

#endif /*__SETUP_USB_PHY_H */
