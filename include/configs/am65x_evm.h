/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Configuration header file for K3 AM654 EVM
 *
 * Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/
 *	Lokesh Vutla <lokeshvutla@ti.com>
 */

#ifndef __CONFIG_AM654_EVM_H
#define __CONFIG_AM654_EVM_H

#include <linux/sizes.h>
#include <environment/ti/mmc.h>
#include <environment/ti/k3_rproc.h>
#include <environment/ti/k3_dfu.h>

/* DDR Configuration */
#define CFG_SYS_SDRAM_BASE1		0x880000000

/* Now for the remaining common defines */
#include <configs/ti_armv7_common.h>

#endif /* __CONFIG_AM654_EVM_H */
