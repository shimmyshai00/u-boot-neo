// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2016 Google, Inc
 * Written by Simon Glass <sjg@chromium.org>
 */

#define LOG_CATEGORY UCLASS_PANEL_BACKLIGHT

#include <common.h>
#include <dm.h>
#include <backlight.h>

int backlight_enable(struct udevice *dev)
{
	const struct backlight_ops *ops = backlight_get_ops(dev);

	if (!ops->enable)
		return -ENOSYS;

	return ops->enable(dev);
}

/* SS: a Rockchip addition */
int backlight_disable(struct udevice *dev)
{
	const struct backlight_ops *ops = backlight_get_ops(dev);

	if (!ops->disable)
		return -ENOSYS;

	return ops->disable(dev);
}

int backlight_set_brightness(struct udevice *dev, int percent)
{
	const struct backlight_ops *ops = backlight_get_ops(dev);

	if (!ops->set_brightness)
		return -ENOSYS;

	return ops->set_brightness(dev, percent);
}

UCLASS_DRIVER(backlight) = {
	.id		= UCLASS_PANEL_BACKLIGHT,
	.name		= "backlight",
};
