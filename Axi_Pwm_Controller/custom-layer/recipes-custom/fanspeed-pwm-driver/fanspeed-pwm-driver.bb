DESCRIPTION = "compile the driver of the pwm fan"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://LICENSE;md5=NothingYet"

SRC_URI = "file://pwm-fan-controller.c \
	   file://pwm-fan-controller.h \
	   file://Makefile"

inherit module

S = "${WORKDIR}"
