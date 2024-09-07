DESCRIPTION = "compile the driver of the pwm fan"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

SRC_URI = "file://pwm-fan-controller.c \
	   file://pwm-fan-controller.h \
	   file://Makefile \
	   file://COPYING"

inherit module

S = "${WORKDIR}"
