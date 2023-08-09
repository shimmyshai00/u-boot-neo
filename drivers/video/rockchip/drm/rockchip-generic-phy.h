/*
 * Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
 * Original written by Jean-Jacques Hiblot  <jjhiblot@ti.com>
 * Adapated by Shimrra Shai  <shimmyshai00@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __ROCKCHIP_PHY_ATTRS_H
#define __GENERIC_PHY_H

#include <generic-phy.h>

#include <rockchip-generic-phy-dp.h>
#include <rockchip-generic-phy-mipi-dphy.h>
#include <rockchip-generic-phy-pcie.h>

/**
 * union rockchip_phy_configure_opts - Opaque generic phy configuration
 *
 * @mipi_dphy: Configuration set applicable for phys supporting
 *	       the MIPI_DPHY phy mode.
 * @dp:	       Configuration set applicable for phys supporting
 *	       the DisplayPort protocol.
 */
union rockchip_phy_configure_opts {
	struct rockchip_phy_configure_opts_mipi_dphy	mipi_dphy;
	struct rockchip_phy_configure_opts_dp		dp;
	struct rockchip_phy_configure_opts_pcie		pcie;
};

/**
 * struct rockchip_phy_attrs - represents phy attributes
 * @bus_width: Data path width implemented by PHY
 * @max_link_rate: Maximum link rate supported by PHY (in Mbps)
 * @mode: PHY mode
 */
struct rockchip_phy_attrs {
	u32			bus_width;
	u32			max_link_rate;
	enum phy_mode		mode;
};

/**
 * struct rockchip_phy - A handle to (allowing control of) a single phy port.
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
struct rockchip_phy {
	struct phy base;
	struct phy_attrs attrs;
};

/**
 * Delegation.
 */
static inline int rockchip_generic_phy_init(struct rockchip_phy *phy)
{
	return generic_phy_init(&phy->base);
}

static inline int rockchip_generic_phy_exit(struct rockchip_phy *phy)
{
	return generic_phy_exit(&phy->base);
}

static inline int rockchip_generic_phy_reset(struct rockchip_phy *phy)
{
	return generic_phy_reset(&phy->base);
}

static inline int rockchip_generic_phy_configure(struct rockchip_phy *phy,
						 union rockchip_phy_configure_opts *opts)
{
	return generic_phy_configure(&phy->base, opts);
}

static inline int rockchip_generic_phy_validate(struct rockchip_phy *phy, enum phy_mode mode,
				       		int submode,
				       		union rockchip_phy_configure_opts *opts)
{
	struct phy_ops const *ops;

	if (!generic_phy_valid(phy))
		return 0;
	ops = phy_dev_ops(phy->dev);

	return ops->validate ? ops->validate(phy, mode, submode, opts) : 0;
}

static inline int rockchip_generic_phy_power_on(struct rockchip_phy *phy)
{
	return 0;
}

static inline int rockchip_generic_phy_power_off(struct rockchip_phy *phy)
{
	return 0;
}

static inline int rockchip_generic_phy_get_by_index(struct udevice *user, int index,
						    struct rockchip_phy *phy)
{
	return 0;
}

static inline int rockchip_generic_phy_get_by_name(struct udevice *user, const char *phy_name,
						   struct rockchip_phy *phy)
{
	return 0;
}

static inline int rockchip_generic_phy_set_mode_ext(struct rockchip_phy *phy, enum phy_mode mode,
						    int submode)
{
	return 0;
}

#define rockchip_generic_phy_set_mode(phy, mode) \
	rockchip_generic_phy_set_mode_ext(phy, mode, 0)

static inline bool rockchip_generic_phy_valid(struct rockchip_phy *phy)
{
	return generic_phy_valid(&phy->base);
}

#endif /*__GENERIC_PHY_H */
