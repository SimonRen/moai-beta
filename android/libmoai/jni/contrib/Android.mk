#================================================================#
# Copyright (c) 2010-2011 Zipline Games, Inc.
# All Rights Reserved.
# http://getmoai.com
#================================================================#

	include $(CLEAR_VARS)

	LOCAL_MODULE 		:= contrib
	LOCAL_ARM_MODE 		:= $(MY_ARM_MODE)

	LOCAL_C_INCLUDES 	:= $(MY_HEADER_SEARCH_PATHS)
	LOCAL_SRC_FILES 	+= $(MY_MOAI_ROOT)/3rdparty/contrib/utf8.c
	LOCAL_CFLAGS		:= -include $(MY_MOAI_ROOT)/src/zipfs/zipfs_replace.h

	include $(BUILD_STATIC_LIBRARY)
 