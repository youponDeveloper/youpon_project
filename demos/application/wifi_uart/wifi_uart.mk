#
#  UNPUBLISHED PROPRIETARY SOURCE CODE
#  Copyright (c) 2016 MXCHIP Inc.
#
#  The contents of this file may not be disclosed to third parties, copied or
#  duplicated in any form, in whole or in part, without the prior written
#  permission of MXCHIP Corporation.
#


#				   ../xlinksrc/xlink_client.c \
#				   ../xlinksrc/xlink_data.c	 \
#				   ../xlinksrc/xlink_md5.c	\
#				   ../xlinksrc/xlink_net.c	\
#				   ../xlinksrc/xlink_process_tcp_data.c	\
#				   ../xlinksrc/xlink_process_udp_data.c	\
#				   ../xlinksrc/xlink_system.c	\
#				   ../xlinksrc/xlink_tcp_client.c	\
#				   ../xlinksrc/xsdk_config.c	\
#				   ../xlinksrc/XlinkByteQueue.c

NAME := App_WiFi_UART

$(NAME)_SOURCES := MICOAppEntrance.c \
                   MICOBonjour.c \
                   SppProtocol.c \
                   UartRecv.c \
                   ../xlinkapp/xlink_config.c \
			       ../xlinkapp/xlink_demo.c \
				   ../xlinkapp/xlink_ringbuf.c \
				   ../xlinkapp/Xlink_PassThroughProtocol.c \
				   ../xlinkapp/Xlink_PassThroughResolve.c \
				   ../xlinkapp/xlink_DataPointProcess.c \
				   ../xlinkapp/OTAupgrade.c \
				   ../xlinkapp/xlinkHttp.c 


$(NAME)_PREBUILT_LIBRARY := ../xlinksrc/Lib_xlink_app_v34700.Cortex-M4.GCC.release.a
                   

