/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
 * Written by Jean-Jacques Hiblot  <jjhiblot@ti.com>
 */

#ifndef __GENERIC_PHY_H
#define __GENERIC_PHY_H

#include <dm/ofnode.h>

/* (Shimrra Shai) Rockchip extensions */
#include <generic-phy-dp.h>
#include <generic-phy-mipi-dphy.h>
#include <generic-phy-pcie.h>
/* --- */

struct ofnode_phandle_args;

enum phy_mode {
	PHY_MODE_INVALID,
	PHY_MODE_USB_HOST,
	PHY_MODE_USB_HOST_LS,
	PHY_MODE_USB_HOST_FS,
	PHY_MODE_USB_HOST_HS,
	PHY_MODE_USB_HOST_SS,
	PHY_MODE_USB_DEVICE,
	PHY_MODE_USB_DEVICE_LS,
	PHY_MODE_USB_DEVICE_FS,
	PHY_MODE_USB_DEVICE_HS,
	PHY_MODE_USB_DEVICE_SS,
	PHY_MODE_USB_OTG,
	PHY_MODE_UFS_HS_A,
	PHY_MODE_UFS_HS_B,
	PHY_MODE_PCIE,
	PHY_MODE_ETHERNET,
	PHY_MODE_MIPI_DPHY,
	PHY_MODE_SATA,
	PHY_MODE_LVDS,
	PHY_MODE_DP
};

/* (Shimrra Shai) These are Rockchip-specific extensions that are needed
 * for the DRM video driver as-is. We should try to get rid of them to 
 * make the driver more self-contained.
 */

/**
 * union phy_configure_opts - Opaque generic phy configuration
 *
 * @mipi_dphy: Configuration set applicable for phys supporting
 *	       the MIPI_DPHY phy mode.
 * @dp:	       Configuration set applicable for phys supporting
 *	       the DisplayPort protocol.
 */
union phy_configure_opts {
	struct phy_configure_opts_mipi_dphy     mipi_dphy;
	struct phy_configure_opts_dp		dp;
	struct phy_configure_opts_pcie		pcie;
};

/**
 * struct phy_attrs - represents phy attributes
 * @bus_width: Data path width implemented by PHY
 * @max_link_rate: Maximum link rate supported by PHY (in Mbps)
 * @mode: PHY mode
 */
struct phy_attrs {
	u32			bus_width;
	u32			max_link_rate;
	enum phy_mode		mode;
};

/* --- end RK-specific stuff --- */

/**
 * struct phy - A handle to (allowing control of) a single phy port.
 *
 * Clients provide storage for phy handles. The content of the structure is
 * managed solely by the PHY API and PHY drivers. A phy struct is
 * initialized by "get"ing the phy struct. The phy struct is passed to all
 * other phy APIs to identify which PHY port to operate upon.
 *
 * @dev: The device which implements the PHY port.
 * @id: The PHY ID within the provider.
 *
 */
struct phy {
	struct udevice *dev;
	unsigned long id;
	struct phy_attrs attrs; /* (SS) Rockchip extension */
};

/*
 * struct udevice_ops - set of function pointers for phy operations
 * @init: operation to be performed for initializing phy (optional)
 * @exit: operation to be performed while exiting (optional)
 * @reset: reset the phy (optional).
 * @power_on: powering on the phy (optional)
 * @power_off: powering off the phy (optional)
 */
struct phy_ops {
	/**
	 * of_xlate - Translate a client's device-tree (OF) phy specifier.
	 *
	 * The PHY core calls this function as the first step in implementing
	 * a client's generic_phy_get_by_*() call.
	 *
	 * If this function pointer is set to NULL, the PHY core will use a
	 * default implementation, which assumes #phy-cells = <0> or
	 * #phy-cells = <1>, and in the later case that the DT cell
	 * contains a simple integer PHY port ID.
	 *
	 * @phy:	The phy struct to hold the translation result.
	 * @args:	The phy specifier values from device tree.
	 * @return 0 if OK, or a negative error code.
	 */
	int	(*of_xlate)(struct phy *phy, struct ofnode_phandle_args *args);

	/**
	 * init - initialize the hardware.
	 *
	 * Hardware intialization should not be done in during probe() but
	 * should be implemented in this init() function. It could be starting
	 * PLL, taking a controller out of reset, routing, etc. This function
	 * is typically called only once per PHY port.
	 * If power_on() is not implemented, it must power up the phy.
	 *
	 * @phy:	the PHY port to initialize
	 * @return 0 if OK, or a negative error code.
	 */
	int	(*init)(struct phy *phy);

	/**
	 * exit - de-initialize the PHY device
	 *
	 * Hardware de-intialization should be done here. Every step done in
	 * init() should be undone here.
	 * This could be used to suspend the phy to reduce power consumption or
	 * to put the phy in a known condition before booting the OS (though it
	 * is NOT called automatically before booting the OS)
	 * If power_off() is not implemented, it must power down the phy.
	 *
	 * @phy:	PHY port to be de-initialized
	 * Return: 0 if OK, or a negative error code
	 */
	int	(*exit)(struct phy *phy);

	/**
	 * reset - resets a PHY device without shutting down
	 *
	 * @phy:	PHY port to be reset
	 *
	 * During runtime, the PHY may need to be reset in order to
	 * re-establish connection etc without being shut down or exit.
	 *
	 * Return: 0 if OK, or a negative error code
	 */
	int	(*reset)(struct phy *phy);

	/**
	 * power_on - power on a PHY device
	 *
	 * @phy:	PHY port to be powered on
	 *
	 * During runtime, the PHY may need to be powered on or off several
	 * times. This function is used to power on the PHY. It relies on the
	 * setup done in init(). If init() is not implemented, it must take care
	 * of setting up the context (PLLs, ...)
	 *
	 * Return: 0 if OK, or a negative error code
	 */
	int	(*power_on)(struct phy *phy);

	/**
	 * power_off - power off a PHY device
	 *
	 * @phy:	PHY port to be powered off
	 *
	 * During runtime, the PHY may need to be powered on or off several
	 * times. This function is used to power off the PHY. Except if
	 * init()/deinit() are not implemented, it must not de-initialize
	 * everything.
	 *
	 * Return: 0 if OK, or a negative error code
	 */
	int	(*power_off)(struct phy *phy);

	/**
	 * configure - configure a PHY device
	 *
	 * @phy:	PHY port to be configured
	 * @params: PHY Parameters, underlying data is specific to the PHY function
	 *
	 * During runtime, the PHY may need to be configured for it's main function.
	 * This function configures the PHY for it's main function following
	 * power_on/off() after being initialized.
	 *
	 * Return: 0 if OK, or a negative error code
	 */
	int	(*configure)(struct phy *phy, void *params);

	/* (SS) Rockchip-specific */
	/**
	 * @validate:
	 *
	 * Optional.
	 *
	 * Used to check that the current set of parameters can be
	 * handled by the phy. Implementations are free to tune the
	 * parameters passed as arguments if needed by some
	 * implementation detail or constraints. It must not change
	 * any actual configuration of the PHY, so calling it as many
	 * times as deemed fit by the consumer must have no side
	 * effect.
	 *
	 * Returns: 0 if the configuration can be applied, an negative
	 * error code otherwise
	 */
	int	(*validate)(struct phy *phy, enum phy_mode mode, int submode,
			    union phy_configure_opts *opts);
	/* --- end RK stuff --- */
	
	/**
	 * set_mode - set PHY device mode
	 *
	 * @phy:	PHY port to be configured
	 * @mode: PHY mode
	 * @submode: PHY submode
	 *
	 * Configure PHY mode (e.g. USB, Ethernet, ...) and submode
	 * (e.g. for Ethernet this can be RGMII).
	 *
	 * Return: 0 if OK, or a negative error code
	 */
	int	(*set_mode)(struct phy *phy, enum phy_mode mode, int submode);

	/**
	 * set_speed - set PHY device speed
	 *
	 * @phy:	PHY port to be configured
	 * @speed: PHY speed
	 *
	 * Configure PHY speed (e.g. for Ethernet, this could be 10 or 100 ...).
	 *
	 * Return: 0 if OK, or a negative error code
	 */
	int	(*set_speed)(struct phy *phy, int speed);
};

/**
 * struct phy_bulk - A handle to (allowing control of) a bulk of phys.
 *
 * Consumers provide storage for the phy bulk. The content of the structure is
 * managed solely by the phy API. A phy bulk struct is initialized
 * by "get"ing the phy bulk struct.
 * The phy bulk struct is passed to all other bulk phy APIs to apply
 * the API to all the phy in the bulk struct.
 *
 * @phys: An array of phy handles.
 * @count: The number of phy handles in the phys array.
 */
struct phy_bulk {
	struct phy *phys;
	unsigned int count;
};

#if CONFIG_IS_ENABLED(PHY)

/**
 * generic_phy_init() - initialize the PHY port
 *
 * @phy:	the PHY port to initialize
 * Return: 0 if OK, or a negative error code
 */
int generic_phy_init(struct phy *phy);

/**
 * generic_phy_init() - de-initialize the PHY device
 *
 * @phy:	PHY port to be de-initialized
 * Return: 0 if OK, or a negative error code
 */
int generic_phy_exit(struct phy *phy);

/**
 * generic_phy_reset() - resets a PHY device without shutting down
 *
 * @phy:	PHY port to be reset
 *Return: 0 if OK, or a negative error code
 */
int generic_phy_reset(struct phy *phy);

/**
 * generic_phy_power_on() - power on a PHY device
 *
 * @phy:	PHY port to be powered on
 * Return: 0 if OK, or a negative error code
 */
int generic_phy_power_on(struct phy *phy);

/**
 * generic_phy_power_off() - power off a PHY device
 *
 * @phy:	PHY port to be powered off
 * Return: 0 if OK, or a negative error code
 */
int generic_phy_power_off(struct phy *phy);

/**
 * generic_phy_configure() - configure a PHY device
 *
 * @phy:	PHY port to be configured
 * @params:	PHY Parameters, underlying data is specific to the PHY function
 * Return: 0 if OK, or a negative error code
 */
int generic_phy_configure(struct phy *phy, void *params);

/**
 * generic_phy_set_mode() - set PHY device mode
 *
 * @phy:	PHY port to be configured
 * @mode: PHY mode
 * @submode: PHY submode
 * Return: 0 if OK, or a negative error code
 */
int generic_phy_set_mode(struct phy *phy, enum phy_mode mode, int submode);

/* (SS: RK hacks) */
int generic_phy_set_mode_ext(struct phy *phy, enum phy_mode mode, int submode);
int generic_phy_set_mode_small(struct phy *phy, enum phy_mode mode);

/**
 * generic_phy_set_speed() - set PHY device speed
 *
 * @phy:	PHY port to be configured
 * @speed: PHY speed
 * Return: 0 if OK, or a negative error code
 */
int generic_phy_set_speed(struct phy *phy, int speed);

/**
 * generic_phy_get_by_index() - Get a PHY device by integer index.
 *
 * @user:	the client device
 * @index:	The index in the list of available PHYs
 * @phy:	A pointer to the PHY port
 *
 * This looks up a PHY device for a client device based on its position in the
 * list of the possible PHYs.
 *
 * example:
 * usb1: usb_otg_ss@xxx {
 *       compatible = "xxx";
 *       reg = <xxx>;
 *   .
 *   .
 *   phys = <&usb2_phy>, <&usb3_phy>;
 *   .
 *   .
 * };
 * the USB2 phy can be accessed by passing index '0' and the USB3 phy can
 * be accessed by passing index '1'
 *
 * Return: 0 if OK, or a negative error code
 */
int generic_phy_get_by_index(struct udevice *user, int index,
			     struct phy *phy);

/**
 * generic_phy_get_by_index_nodev() - Get a PHY device by integer index
 * without a device
 *
 * @node:	The client ofnode.
 * @index:	The index in the list of available PHYs
 * @phy:	A pointer to the PHY port
 *
 * This is a version of generic_phy_get_by_index() that does not use a device.
 *
 * This looks up a PHY device for a client device based on its ofnode and on
 * its position in the list of the possible PHYs.
 *
 * example:
 * usb1: usb_otg_ss@xxx {
 *       compatible = "xxx";
 *       reg = <xxx>;
 *   .
 *   .
 *   phys = <&usb2_phy>, <&usb3_phy>;
 *   .
 *   .
 * };
 * the USB2 phy can be accessed by passing index '0' and the USB3 phy can
 * be accessed by passing index '1'
 *
 * Return: 0 if OK, or a negative error code
 */
int generic_phy_get_by_index_nodev(ofnode node, int index, struct phy *phy);

/**
 * generic_phy_get_by_name() - Get a PHY device by its name.
 *
 * @user:	the client device
 * @phy_name:	The name of the PHY in the list of possible PHYs
 * @phy:	A pointer to the PHY port
 *
 * This looks up a PHY device for a client device in the
 * list of the possible PHYs based on its name.
 *
 * example:
 * usb1: usb_otg_ss@xxx {
 *       compatible = "xxx";
 *       reg = <xxx>;
 *   .
 *   .
 *   phys = <&usb2_phy>, <&usb3_phy>;
 *   phy-names = "usb2phy", "usb3phy";
 *   .
 *   .
 * };
 * the USB3 phy can be accessed using "usb3phy", and USB2 by using "usb2phy"
 *
 * Return: 0 if OK, or a negative error code
 */
int generic_phy_get_by_name(struct udevice *user, const char *phy_name,
			    struct phy *phy);

/**
 * generic_phy_get_bulk - Get all phys of a device.
 *
 * This looks up and gets all phys of the consumer device; each device is
 * assumed to have n phys associated with it somehow, and this function finds
 * and gets all of them in a separate structure.
 *
 * @dev:	The consumer device.
 * @bulk	A pointer to a phy bulk struct to initialize.
 * Return: 0 if OK, or a negative error code.
 */
int generic_phy_get_bulk(struct udevice *dev, struct phy_bulk *bulk);

/**
 * generic_phy_init_bulk() - Initialize all phys in a phy bulk struct.
 *
 * @bulk:	A phy bulk struct that was previously successfully requested
 *		by generic_phy_get_bulk().
 * Return: 0 if OK, or negative error code.
 */
int generic_phy_init_bulk(struct phy_bulk *bulk);

/**
 * generic_phy_exit_bulk() - de-initialize all phys in a phy bulk struct.
 *
 * @bulk:	A phy bulk struct that was previously successfully requested
 *		by generic_phy_get_bulk().
 * Return: 0 if OK, or negative error code.
 */
int generic_phy_exit_bulk(struct phy_bulk *bulk);

/**
 * generic_phy_power_on_bulk() - Power on all phys in a phy	bulk struct.
 *
 * @bulk:	A phy bulk struct that was previously successfully requested
 *		by generic_phy_get_bulk().
 * Return: 0 if OK, or negative error code.
 */
int generic_phy_power_on_bulk(struct phy_bulk *bulk);

/**
 * generic_phy_power_off_bulk() - Power off all phys in a phy bulk struct.
 *
 * @bulk:	A phy bulk struct that was previously successfully requested
 *		by generic_phy_get_bulk().
 * Return: 0 if OK, or negative error code.
 */
int generic_phy_power_off_bulk(struct phy_bulk *bulk);

/**
 * generic_setup_phy() - Get, initialize and power on phy.
 *
 * @dev:	The consumer device.
 * @phy:	A pointer to the PHY port
 * @index:	The index in the list of available PHYs
 *
 * Return: 0 if OK, or negative error code.
 */
int generic_setup_phy(struct udevice *dev, struct phy *phy, int index);

/**
 * generic_shutdown_phy() - Power off and de-initialize phy.
 *
 * @phy:	A pointer to the PHY port.
 *
 * Return: 0 if OK, or negative error code.
 */
int generic_shutdown_phy(struct phy *phy);

#else /* CONFIG_PHY */

static inline int generic_phy_init(struct phy *phy)
{
	return 0;
}

static inline int generic_phy_exit(struct phy *phy)
{
	return 0;
}

static inline int generic_phy_reset(struct phy *phy)
{
	return 0;
}

static inline int generic_phy_power_on(struct phy *phy)
{
	return 0;
}

static inline int generic_phy_power_off(struct phy *phy)
{
	return 0;
}

static inline int generic_phy_configure(struct phy *phy, void *params)
{
	return 0;
}

static inline int generic_phy_set_mode(struct phy *phy, enum phy_mode mode, int submode)
{
	return 0;
}

static inline int generic_phy_set_speed(struct phy *phy, int speed)
{
	return 0;
}

static inline int generic_phy_get_by_index(struct udevice *user, int index,
			     struct phy *phy)
{
	return 0;
}

static inline int generic_phy_get_by_name(struct udevice *user, const char *phy_name,
			    struct phy *phy)
{
	return 0;
}

static inline int
generic_phy_get_bulk(struct udevice *dev, struct phy_bulk *bulk)
{
	return 0;
}

static inline int generic_phy_init_bulk(struct phy_bulk *bulk)
{
	return 0;
}

static inline int generic_phy_exit_bulk(struct phy_bulk *bulk)
{
	return 0;
}

static inline int generic_phy_power_on_bulk(struct phy_bulk *bulk)
{
	return 0;
}

static inline int generic_phy_power_off_bulk(struct phy_bulk *bulk)
{
	return 0;
}

static inline int generic_setup_phy(struct udevice *dev, struct phy *phy, int index)
{
	return 0;
}

static inline int generic_shutdown_phy(struct phy *phy)
{
	return 0;
}

/* (SS) Rockchip HACK - don't want this here! */
static inline int generic_phy_set_mode_ext(struct phy *phy, enum phy_mode mode, int submode)
{
	return 0;
}

static inline int generic_phy_set_mode_small(struct phy *phy, enum phy_mode mode)
{
	return 0;
}

#endif /* CONFIG_PHY */

/**
 * generic_phy_valid() - check if PHY port is valid
 *
 * @phy:	the PHY port to check
 * Return: TRUE if valid, or FALSE
 */
static inline bool generic_phy_valid(struct phy *phy)
{
	return phy && phy->dev;
}

#endif /*__GENERIC_PHY_H */
