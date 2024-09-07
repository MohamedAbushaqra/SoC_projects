SUMMARY = "bitbake file to manage the cli application and the corresponding systemd service"
DESCRIPTION = "compiles the cpp cli application and deploy the corresponding systemd"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

SRC_URI = "file://pwm-fan-controller-userapp.cpp \
	   file://pwm-fan-controller.h \
           file://fan-speed-controller.service" \
	   file://COPYING"

S = "${WORKDIR}"

inherit systemd
	
SYSTEMD_SERVICE:${PN} = "fan-speed-controller.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_compile() {
    ${CXX} ${CXXFLAGS} ${LDFLAGS} -o fanspeed-cli ${WORKDIR}/pwm-fan-controller-userapp.cpp
}


do_install() {
    install -d ${D}${bindir}
    install -m 0755 fanspeed-cli ${D}${bindir}/fanspeed-cli

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${WORKDIR}/fan-speed-controller.service ${D}${systemd_system_unitdir}/fan-speed-controller.service
}

FILES:${PN} += "${bindir}/fanspeed-cli"
FILES:${PN} += "${systemd_system_unitdir}/fan-speed-controller.service"
