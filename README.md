# SoC_projects
A collection of different educational projects targeting AMD Xilinx SoCs 

This repository contains various projects developed for educational purposes. The projects focus on ZynqMP SoCs and Zynq-7000 devices. After developing the system design i will be using mostly petalinux for the development purposes and to test the designs. Stand alone using Vitis is also possible and even easier since we have direct access to the registers, but the aim is also to take a look on how drivers work under linux.

************* Petalinux Installation***************
- Please do the following on a linux OS that is supported by petalinux 2023.1 (I will be using ubuntu 22.04)
- In the root of this repository there is a "petalinux_installation.sh" shell script that can be used to install the petalinux SDK.
- First download the "petalinux-v2023.1-final-installer.run" from the official AMD Xilinx website and place it in the same directory as the "petalinux_installation.sh"
- Run "sudo chmod +x petalinux_installation.sh"
- Run "./petalinux_installation.sh --petalinuxversion 2023.1"
- All needed packages and dependencies will be installed on the system, as well as the petalinux SDK.
- Once the instalaltion is done we can source the SDK settings in order to use it. 
- Run "source /opt/petalinux/settings.sh" 
- Verify the installation by typing petalinux in the terminal and using the Tab key to check if the Petalinux commands are available.
