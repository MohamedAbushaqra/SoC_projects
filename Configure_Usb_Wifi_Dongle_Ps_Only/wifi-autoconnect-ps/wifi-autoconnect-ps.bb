SUMMARY = "bitbake file to copy systemd service and package it"
DESCRIPTION = "copy thee service unit for the auto wifi settings and enable it"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

SRC_URI = "file://auto-connect.service \
           file://COPYING"

S = "${WORKDIR}"

inherit systemd
	
SYSTEMD_SERVICE:${PN} = "auto-connect.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install() {
    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/auto-connect.service ${D}${systemd_system_unitdir}/auto-connect.service
}

FILES:${PN} += "${systemd_system_unitdir}/auto-connect.service"
