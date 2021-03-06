/*
 * mvisp.h
 *
 * Marvell DxO ISP - DMA module
 *
 * Copyright:  (C) Copyright 2011 Marvell International Ltd.
 *              Henry Zhao <xzhao10@marvell.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */


#ifndef ISP_USER_H
#define ISP_USER_H

#include <linux/videodev2.h>
#include <linux/types.h>

struct v4l2_dxoipc_set_fb {
	void	*virAddr[4];
	int		phyAddr[4];
	int		size[4];
	int		fbcnt;
	int		burst_read;
	int		burst_write;
	int		flush_fifo_by_timer;
};

struct v4l2_ispdma_timeinfo {
	long	sec;
	long	usec;
	long	delta;
};

struct v4l2_dxoipc_ipcwait {
	int				timeout;
	struct v4l2_ispdma_timeinfo tickinfo;
};

struct v4l2_dxoipc_streaming_config {
	int		enable_in;
	int		enable_disp;
	int		enable_codec;
	int		enable_fbrx;
	int		enable_fbtx;
};

struct v4l2_dxoipc_config_codec {
	int		vbnum;
	int		vbsize;
	int		dma_burst_size;
};

struct v4l2_sensor_get_driver_name {
	char	driver[16];
};

struct v4l2_sensor_register_access {
	unsigned long	reg;
	unsigned long	value;
};

/*Normal: */
/* 1. Dummy buffer will reuse filled buffer.*/
/* 2. Driver will hold one buffer in queue serve as dummy buffer.*/
/*Still: */
/* 1. Dummy buffer will only use idle buffer.*/
/* 2. Driver will not hold any buffer in queue.*/
enum ispvideo_capture_mode {
	ISPVIDEO_NORMAL_CAPTURE = 0,
	ISPVIDEO_STILL_CAPTURE,
};

enum ispdma_port {
	ISPDMA_PORT_CODEC = 0,
	ISPDMA_PORT_DISPLAY,
	ISPDMA_PORT_FBRX,
	ISPDMA_PORT_FBTX,
	ISPDMA_PORT_INPUT,
};

struct v4l2_ispdma_capture_mode {
	enum ispdma_port port;
	enum ispvideo_capture_mode mode;
};

struct v4l2_ispdma_reset {
	unsigned long	param;
};

struct v4l2_ccic_config_mipi {
	int start_mipi;
};

#define VIDIOC_PRIVATE_DXOIPC_SET_FB \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 1, struct v4l2_dxoipc_set_fb)
#define VIDIOC_PRIVATE_DXOIPC_WAIT_IPC \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 2, struct v4l2_dxoipc_ipcwait)
#define VIDIOC_PRIVATE_DXOIPC_SET_STREAM \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 3, struct v4l2_dxoipc_streaming_config)
#define VIDIOC_PRIVATE_SENSER_GET_DRIVER_NAME \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 4, struct v4l2_sensor_get_driver_name)
#define VIDIOC_PRIVATE_SENSER_REGISTER_SET \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 5, struct v4l2_sensor_register_access)
#define VIDIOC_PRIVATE_SENSER_REGISTER_GET \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 6, struct v4l2_sensor_register_access)
#define VIDIOC_PRIVATE_DXOIPC_CONFIG_CODEC \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 7, struct v4l2_dxoipc_config_codec)
#define VIDIOC_PRIVATE_ISPDMA_CAPTURE_MODE \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 8, struct v4l2_ispdma_capture_mode)
#define VIDIOC_PRIVATE_ISPDMA_RESET \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 9, struct v4l2_ispdma_reset)
#define VIDIOC_PRIVATE_ISPDMA_GETDELTA \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 10, struct v4l2_ispdma_timeinfo)
#define VIDIOC_PRIVATE_CCIC_CONFIG_MIPI \
	_IOWR('V', BASE_VIDIOC_PRIVATE + 11, struct v4l2_ccic_config_mipi)

#endif	/* ISP_USER_H */
