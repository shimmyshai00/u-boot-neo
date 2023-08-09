// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2023
 * written by Shimrra Shai  <shimmyshai00@gmail.com>
 */

#include <common.h>
#include <log.h>
#include <asm/global_data.h>
#include <dm/ofnode.h>
#include <fdt_support.h>
#include <linux/libfdt.h>

DECLARE_GLOBAL_DATA_PTR;

void rockchip_display_fixup(void *blob);

int ft_system_setup(void *blob, struct bd_info *bd)
{
#ifdef CONFIG_DRM_ROCKCHIP
	rockchip_display_fixup(blob);
#endif

	return 0;
}
