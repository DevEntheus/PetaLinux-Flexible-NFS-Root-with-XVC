# PetaLinux Flexible NFS Root with XVC

 1. [Introduction](#introduction)
 2. [Overview](#overview)
 3. [Create Ubuntu 22.04.2 VM](#create-ubuntu-22042-vm)
     * [Install the required packages](#install-the-required-packages)
 4. [TFTP Server](#tftp-server)
     * [Install TFTP Server](#install-tftp-server)
     * [Create TFTP Server Directory](#create-tftp-server-directory)
     * [Configure TFTP Server](#configure-tftp-server)
     * [Restart TFTP Server](#restart-tftp-server)
     * [Verify TFTP Server](#verify-tftp-server)
     * [Start TFTP Server](#start-tftp-server)
 5. [NFS Server](#nfs-server)
     * [Install NFS Server](#install-nfs-server)
     * [Create NFS Root Directory](#create-nfs-root-directory)
     * [Export NFS Root (***important***)](#export-nfs-root-important)
     * [Restart NFS Server](#restart-nfs-server)
     * [Configure the Firewall](#configure-the-firewall)
     * [Verify NFS Server](#verify-nfs-server)
 6. [PetaLinux Tools](#petalinux-tools)
     * [Install the PetaLinux Tools](#install-the-petalinux-tools)
 7. [Prepare Block Design (BD)](#prepare-block-design)
     * [Add to BD Debug Bridge](#add-to-bd-debug-bridge)
     * [Add to BD System ILA](#add-to-bd-system-ila)
 8. [PetaLinux Project](#petalinux-project)
     * [Create a PetaLinux Project](#create-a-petalinux-project)
     * [Device Tree Configuration](#device-tree-configuration)
     * [Hardware Configuration](#hardware-configuration)
     * [Root File System Configuration](#root-file-system-configuration)
     * [Kernel configuration](#kernel-configuration)
     * [Build PetaLinux Project](#build-petalinux-project)
     * [Packaging a Petalinux Project](#packaging-a-petalinux-project)
 9. [Xilinx Virtual Cable (XVC)](#xilinx-virtual-cable-xvc)
     * [Overview XVC](#overview-xvc)
     * [Hardware Configuration](#hardware-configuration)
     * [Edit Petalinux Project](#edit-petalinux-project)
     * [Create Kernel Module (xvc-driver)](#create-kernel-module-xvc-driver)
     * [Create Application (xvcserver)](#create-application-xvcserver)
     * [Start XVC Driver](#start-xvc-driver)
     * [Start XVC Server](#start-xvc-server)
10. [Remote Peripheral Testing (RPT)](#remote-peripheral-testing-rpt)
     * [Overview RPT](#overview-rpt)
     * [Edit Petalinux Project](#edit-petalinux-project)
     * [Create Application (rptserver)](#create-application-rptserver)
     * [Start RPT Server](#start-rpt-server)
11. [Prepare SD Card](#prepare-sd-card)
12. [Prepare NFS Root](#prepare-nfs-root)
13. [The First Boot](#the-first-boot)
     * [U-Boot CLI](#u-boot-cli)
14. [Setup Petalinux](#setup-petalinux)
     * [Configure .bashrc](#configure-bashrc)
     * [DNF Update](#dnf-update)
     * [Install Conda](#install-conda)
     * [Create Conda Environment](#create-conda-environment)
     * [Install necessary packages](#install-necessary-packages)
15. [Remote development with VS Code](#remote-development-with-vs-code)     
16. [FPGA Manager](#fpga-manager)
17. [Hardware Manager](#hardware-manager)
     * [Hardware Targets](#hardware-targets)
     * [Hardware ILA](#hardware-ila)
18. [Example of Block Design](#example-of-block-design)
19. [Glossary](#glossary)


## Introduction
**Overview of the Project: Configuring PetaLinux with NFS Root File System and XVC**

Embedded systems landscape, the seamless integration of hardware and software components is vital for efficient development and deployment. This project aims to streamline the process by configuring a PetaLinux system with an NFS (Network File System) root file system and integrating XVC (Xilinx Virtual Cable) for remote debugging capabilities. Below is an overview of the project's key components and objectives:

**NFS Root File System:**

* Purpose: NFS serves as a distributed file system protocol, enabling network-based access to files and directories.
* Objective: To set up a PetaLinux system with NFS as the root file system, providing centralized storage and simplified maintenance.
* Benefits: Centralized management, easy updates, and the ability to boot embedded devices directly from a network-shared file system.

**XVC (Xilinx Virtual Cable):**

* Purpose: XVC facilitates remote debugging and monitoring of FPGA or SoC devices without the need for physical JTAG cables.
* Objective: Integration of XVC into the PetaLinux project to enable remote debugging capabilities.
* Benefits: Flexible debugging workflows, real-time monitoring of internal signals, and enhanced efficiency in hardware-software co-design.

**Project Setup Steps:**

* TFTP and NFS Server Configuration: Installation and configuration of TFTP and NFS servers to facilitate file sharing and system booting.
* NFS Root File System Preparation: Creation of a robust NFS root file system with proper permissions and configurations for seamless access.
* XVC Integration: Incorporation of XVC driver and server components into the PetaLinux project for remote debugging purposes.
* Hardware Configuration: Setup of debug bridges and ILA cores within the hardware design for real-time monitoring and debugging capabilities.

**Objectives:**

* Establishing a comprehensive development environment conducive to efficient hardware-software co-design and debugging processes.
* Enabling remote debugging capabilities through the integration of XVC, enhancing flexibility and scalability in the development workflow.
* Streamlining file system management and access by configuring NFS as the root file system, promoting centralized storage and simplified maintenance.

**Conclusion:**

* By following the outlined steps and configurations, can create a robust PetaLinux environment with NFS as the root file system and integrated XVC for remote debugging. This setup empowers with enhanced flexibility, scalability, and efficiency in designing and debugging FPGA or SoC-based applications, ultimately accelerating the development cycle and improving overall productivity.

## Overview
The project aims to configure a **PetaLinux system** with an **NFS root file system** and integrate **XVC** for remote debugging capabilities of **FPGA** or **SoC** devices. It involves creating a **Virtual Machine (VM) with Ubuntu 22.04.2 LTS**, installing and configuring **TFTP** and **NFS** servers, setting up **PetaLinux Tools 2023.2**, creating a **PetaLinux project**, preparing the block design using **Xilinx Vivado 2023.2.1**, configuring the root file system, integrating **XVC** for remote debugging, and preparing the **SD card** and **NFS root** for booting the system. Additionally, the project includes setting up **FPGA Manager** and **Hardware Manager** for configuring the bitstream and monitoring the hardware interfaces, aiming to streamline the development and debugging process by providing a flexible and efficient environment for hardware-software co-design and debugging workflows.

## Create Ubuntu 22.04.2 Virtual Machine (VM)
### Minimum requirements to run PetaLinux Tools 2023.2:
* **8 GB RAM** (recommended minimum for AMD tools)
* **2 GHz CPU** clock or equivalent (minimum of eight cores)
* **100 GB** free disk space
* **Ubuntu 22.04.2 LTS (64-bit)** (latest version unsupported)
* **Internet connection**
* System shell **/bin/bash** or **/bin/sh**

### Install the required packages
Before installing the **PetaLinux Tools**, ensure that the required packages are installed on the **Ubuntu 22.04.2 VM**. Change the system shell to **/bin/bash** or **/bin/sh** if necessary.

Check the current shell:
```bash
echo $SHELL
```
Check the available shells:
```bash
cat /etc/shells
```
Change the shell to /bin/bash:
```bash
sudo chsh -s /bin/bash
```
Update the package list:
```bash
sudo apt update
```
#### **Don't UPGRADE the packages!**

Copy "[plnx-env-setup.sh](PetlaLinux_Tools/plnx-env-setup.sh)" to the home directory and run the following command to install the required packages:
```bash
sudo ~/plnx-env-setup.sh
```

## TFTP Server
### Install TFTP Server
```bash
sudo apt update
```
```bash
sudo apt install tftpd-hpa
```
### Create TFTP Server Directory
```bash
sudo mkdir /tftpboot
```
#### Change Ownership
```bash
sudo chown -R nobody:nogroup /tftpboot
```
#### Change Permissions
```bash
sudo chmod -R 777 /tftpboot
```
### Configure TFTP Server
Edit the **/etc/default/tftpd-hpa** file:
```bash
sudo nano /etc/default/tftpd-hpa
```
Change the file to the following:
```text
TFTP_USERNAME="tftp"
TFTP_DIRECTORY="/tftpboot"
TFTP_ADDRESS="0.0.0.0:69"
TFTP_OPTIONS="--secure"
```
#### Restart TFTP Server
After editing the configuration file, restart the TFTP server:
```bash
sudo systemctl restart tftpd-hpa
```
#### Verify TFTP Server
To verify that the TFTP server is running, run:
```bash
sudo systemctl status tftpd-hpa
```
#### Start TFTP Server
If the TFTP server is not running, start it:
```bash
sudo systemctl start tftpd-hpa
```
#### Start TFTP Server on Boot
To start the TFTP server on boot, run:
```bash
sudo systemctl enable tftpd-hpa
```

## NFS Server
Network File System (NFS) is a distributed file system protocol allowing a user on a client computer to access files over a network in a manner similar to how local storage is accessed. An NFS server is a system that hosts shared directories, making them available to clients on the network.
### Install NFS Server
To install the NFS server, run the following command:
```bash
sudo apt install nfs-kernel-server
```
### Create NFS Root Directory
Create a directory for the NFS server:
```bash
sudo mkdir /tftpboot/nfsroot
```
### Set Permissions
Set ownership of NFS root directory:
```bash
sudo chown -R nobody:nogroup /tftpboot/nfsroot
``` 
Set permissions of NFS root directory:
```bash
sudo chmod -R 777 /tftpboot/nfsroot
```
### Export NFS Root (***important***)
Edit the NFS server configuration:
```bash
sudo nano /etc/exports
```
Add the following line(s) to the configuration file:
```bash
/tftpboot/nfsroot 192.168.1.0/24(rw,sync,no_subtree_check,no_root_squash)
```
* **/tftpboot/nfsroot** - The directory to export
* **192.168.1.0/24** - The IP address range that can access the NFS share
* **rw** - Read/Write access
* **sync** - Synchronous writes
* **no_subtree_check** - Disable subtree checking
* **no_root_squash** - Disable root squashing

To apply the changes, run the following
```bash
sudo exportfs -a
```
### Restart NFS Server
After changing the configuration file, restart the NFS server with the following command:
```bash
sudo systemctl restart nfs-kernel-server
```
### Configure the Firewall
If the firewall is enabled, allow NFS traffic through the firewall:
```bash
sudo ufw allow from 192.168.1.0/24 to any port nfs
```
### Enable the Firewall
To enable the firewall, run the following command:
```bash
sudo ufw enable
```
### Firewall Status
To check the status of the firewall, run:
```bash
sudo ufw status
```
### Verify NFS Server
To verify that the NFS server is running, run:
```bash
sudo systemctl status nfs-kernel-server
```
```bash
sudo showmount -e
```

## PetaLinux Tools
### Download the PetaLinux Tools
Download the PetaLinux Tools installer from the [Xilinx Download Center](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools.html) to the home directory.

### Create a directory for the PetaLinux Tools
```bash
mkdir -p ~/Xilinx/petalinux/2023.2
```
### Make the installer executable
```bash
chmod +x ~/<petalinux_installer>.run
```
### Install the PetaLinux Tools
Run the installer. The PetaLinux tools need to be installed as non-root users:
```bash
./<petalinux_installer>.run -d ~/Xilinx/petalinux/2023.2
```
### Launch PetaLinux Tools when terminal is started
Edit the **/etc/bash.bashrc** file::
```bash
sudo nano /etc/bash.bashrc
```
Add the following lines to the begin of the file:
```text
# Start Petalinux 2023.2
source ~/Xilinx/petalinux/2023.2/settings.sh
```
If everything is set up correctly, the PetaLinux Tools should be available in the terminal after restarting the terminal and printing the following message:
```text
PetaLinux environment set to '/home/<user_name>/Xilinx/petalinux/2023.2'
INFO: Checking free disk space
INFO: Checking installed tools
INFO: Checking installed development libraries
INFO: Checking network and other services
```

## Prepare Block Design
Create new Project in **Xilinx Vivado 2023.2.1** and add the necessary IP blocks to the Block Design see the ***[Block Design Example](#example-of-block-design)***.
### Add to Block Design the System ILA IP
Configure the **System ILA** to capture the interfaces of interest. The **System ILA** is used to monitor and debug the hardware interfaces in real-time.
![System ILA configuration](Images/system_ila.jpg "System ILA configuration")
Add the **System ILA** to the **Block Design**, as shown in the **[Block Design Example](#example-of-block-design)**.
After the **Block Design** completion, validate the design implementation and generate the bitstream. Export the hardware platform including the bitstream. Copy the hardware platform to the **Ubuntu 22.04.2 VM** home directory.

## PetaLinux Project
### Create a PetaLinux Project
Create a new PetaLinux project with the following command:
```bash
petalinux-create --type project --template zynqMP --name <project name>
```
### Device Tree Configuration
To use Debug Bridge, change the **system-user.dtsi** device tree at the end of the file of the PetaLinux project in **<plnx-proj-root>/project-spec/meta-user/recipes-bsp/device-tree/files** to change the compatible string to match that driver.
```text
&sdhci0 {
       no-1-8-v;
       disable-wp;
};

&sdhci1 {
       no-1-8-v;
       disable-wp;
};
```
### Hardware Configuration
Use the following command to import the hardware platform into the PetaLinux project:
```bash
petalinux-config --get-hw-description ~/<hardware_platform>.xsa
```
And configure as follows:
```text
-> Subsystem AUTO Hardware Settings  --->
	Ethernet Settings  --->
		[*] Randomise MAC address

-> FPGA Manager --->
	[*] Fpga Manager
	
-> Image Packaging Configuration --->
	Root filesystem type (NFS) --->
		(X) NFS
	(/tftpboot/nfsroot) Location of NFS root directory --->
		/tftpboot/nfsroot
	[*] Copy final images to tftpboot
	(/tftpboot) tftpboot directory --->
	    /tftpboot
```
After configuring the hardware, save the configuration and exit the menu.
### Root File System Configuration
To configure the root file system, run the following command:
```bash
petalinux-config -c rootfs
```
Configure the root file system as follows:
```text
-> Filesystem Packages --->
    base ---> 
        dnf --->
            [*] dnf

-> Image Features --->
    [*] package-management

-> PetaLinux RootFS Settings --->
    (root:root;<username>:<pass>;) Add Extra Users --->
        (root:root;<username>:<pass>)
    (<username>:audio,video,aie,input;) Add Users to Groups
        (<username>:audio,video,aie,input)
    (vtuser) Add Users to Sudo users --->
        (vtuser)    
```
### Kernel configuration
Check kernel Configuration in accordance with PetaLinux Tools Documentation: [PetaLinux Tools Documentation: Reference Guide "Configuring NFS Boot"](https://docs.amd.com/r/en-US/ug1144-petalinux-tools-reference-guide/Configuring-NFS-Boot) section.
### Build PetaLinux Project
To build the PetaLinux project, run the following command:
```bash
petalinux-build
```
### Packaging a Petalinux Project
To package the PetaLinux project, run the following command:
```bash
petalinux-package --boot --format BIN --fsbl images/linux/zynqmp_fsbl.elf --u-boot images/linux/u-boot.elf --pmufw images/linux/pmufw.elf --fpga images/linux/system.bit --force
```
After packaging the project, all necessary files will be available in the **images/linux** directory also if **TFTP server** is configured correctly, the files will be copied to the **/tftpboot** directory.

## Xilinx Virtual Cable (XVC)
### Overview XVC
Virtual Cable (XVC) is a TCP/IP-based protocol that acts like a JTAG cable and provides a means to access and debug your FPGA or SoC design without using a physical cable see [Xilinx Virtual Cable GitHub](https://github.com/Xilinx/XilinxVirtualCable).
### Hardware Configuration
#### Add to Block Design the Debug Bridge IP
For this project, the Debug Bridge type is configured as AXI to BSCAN. This bridge type is intended for designs that use Xilinx Virtual Cable (XVC) to remotely debug on FPGA or SoC device through Ethernet or other interfaces without the need for JTAG cable. In this mode, the Debug Bridge receives XVC Commands via AXI4-Lite interface.
![Debug bridge configuration](Images/debug_bridge.jpg "Debug bridge configuration")
Add the **Debug Bridge** to the Block Design, as shown in the ***[Block Design Example](#block-design-example)***.
After the **Block Design** completion, validate the design implementation and generate the bitstream. Export the hardware platform including the bitstream. Copy the hardware platform to the **Ubuntu 22.04.2 VM** home directory.
### Edit Petalinux Project
#### Device Tree Configuration
To use Debug Bridge, change the **system-user.dtsi** device tree at the end of the file of the PetaLinux project in **<plnx-proj-root>/project-spec/meta-user/recipes-bsp/device-tree/files** to change the compatible string to match that driver.
```text
&debug_bridge_0 {
        compatible = "xlnx,xvc";
};
```
Use the following command to import the hardware platform into the PetaLinux project to configure the XVC driver:
```bash
petalinux-config --get-hw-description ~/<hardware_platform>.xsa
```
### Create Kernel Module (xvc-driver)
Create a new kernel module for the XVC driver:
```bash
petalinux-create -t modules --name xvc-driver --enable
```
Copy following files: ***xvc_driver_base.c***, ***xvc_driver.c***, ***xvc_driver.h***, ***xvc_ioctl.h***, ***xvc_user_config.h*** into recipe to build the kernel module in the ***project-spec/meta-user/recipes-modules/xvc-driver/files*** directory. Files can be found in the [XVC_ZynqMP/xvc-driver](XVC_ZynqMP/xvc-driver/files) directory or copy and replace entire ***xvc-driver** folder.

Change the SRC_URI BitBake variable in the ***project-spec/meta-user/recipes-modules/xvc-driver/xvc-driver.bb*** recipe file to:
```BitBake
SRC_URI = "file://Makefile \
           file://COPYING \
           file://xvc_driver_base.c \
           file://xvc_driver.c \
           file://xvc_driver.h \
           file://xvc_ioctl.h \
           file://xvc_user_config.h \
          "
```
Change the first two lines of the Makefile in the directory **project-spec/meta-user/recipes-modules/xvc-driver/files** to:
```Makefile
obj-m := xvc-driver.o
xvc-driver-objs := xvc_driver.o xvc_driver_base.o
```
Add the following line to the **xvc_driver.h** file (in folder [XVC_ZynqMP/xvc-driver](XVC_ZynqMP/xvc-driver/files) all files already modified):
```C
#define LOG_PREFIX
```
Before building the module, configure the root file system with the following command:
```bash
petalinux-config -c rootfs
```
Check if the module is enabled in the configuration in:
```bash
user packages ---> 
    [*] xvc-driver
```
To compile and install module run following commands:
```bash
petalinux-build -c kernel
``` 
```bash
petalinux-build -c xvc-driver
```
Also need to rebuild PetaLinux bootable images run this command:
```bash
petalinux-build -c rootfs
```
### Create Application (xvcserver)
* **Avoid using capital letters and underscore '_' when creating application name**
Create XVC server application with the following command:
```bash
petalinux-create -t apps --name xvcserver --enable
```
Copy the following files: ***xvcserver.c***, ***xvc_ioctl.h*** and ***Makefile*** into the recipe to build the **XVC server** application in the ***project-spec/meta-user/recipes-apps/xvcserver/files*** directory. Files can be found in the [XVC_ZynqMP/xvcserver](XVC_ZynqMP/xvcserver/files) directory or copy and replace entire ***xvcserver*** folder.

Change the SRC_URI BitBake variable in the ***project-spec/meta-user/recipes-apps/xvcserver/xvcserver.bb*** recipe file (in folder [XVC_ZynqMP/xvcserver](XVC_ZynqMP/xvcserver/files) all files already modified) to:
```BitBake
SRC_URI = "file://xvcserver.c \
           file://xvc_ioctl.h \
           file://Makefile \
          "
```
also add the following lines:
```BitBake
DEPENDS = "\
    xvc-driver \
    "
```
Check if the xvcserver application is enabled in the configuration:
```bash
    user packages ---> 
        [*] xvcserver
```
To build xvcserver application, run the following command:
```bash
petalinux-build -c xvcserver
```
Build the PetaLinux project with the following command:
```bash
petalinux-build
```
### Start XVC Driver
To start the XVC driver, run the following command:
```bash
sudo modprobe xvc-driver
```
### Start XVC Server
To start the XVC server, run the following command:
```bash
sudo xvcserver &
```

## Remote Peripheral Testing (RPT)
### Overview RPT
Remote Peripheral Testing (RPT) is a simple server-client application that allows the user to access and control the peripherals of an FPGA or SoC device remotely. The server application runs on the embedded system, while the client application runs on the host machine, enabling the user to interact with the peripherals through a network connection. RPT provides a convenient way to test and validate the functionality of the peripherals without the need for physical access to the device.
### Edit Petalinux Project
### Create Application (rptserver)
Create RPT server application with the following command:
```bash
petalinux-create -t apps --name rptserver --enable
```
Copy the following files: **rptserver.c**, **gpio.c**, **mmio.c**, **mmio.h** and **Makefile** into the recipe to build the RPT server application in the **project-spec/meta-user/recipes-apps/rptserver/files** directory. Files can be found in the [XVC_ZynqMP/rptserver](XVC_ZynqMP/rptserver/files) directory or copy and replace entire **rptserver** folder.

Change the SRC_URI BitBake variable in the **project-spec/meta-user/recipes-apps/rptserver/rptserver.bb** recipe file (in folder [XVC_ZynqMP/rptserver](XVC_ZynqMP/rptserver/files) all files already modified) to:
```BitBake
SRC_URI = "file://Makefile \
           file://rptserver.c \
           file://gpio.c \
           file://mmio.c \
           file://mmio.h \
          "
```
Check if the rptserver application is enabled in the configuration in:
```bash
user packages ---> 
    [*] rptserver
```
To build rptserver application, run the following command:
```bash
petalinux-build -c rptserver
```
Also, it is necessary to rebuild PetaLinux bootable images. To do this, run the following:
```bash
petalinux-build -c rootfs
```

Then, build the PetaLinux project with the following command:
```bash
petalinux-build
```
### Start RPT Server
To start the RPT server, run the following command:
```bash
sudo rptserver
```

## Prepare SD Card
To prepare the **SD card**, follow the steps below:
* Format the **SD card** with the ****FAT32** file system, partition should be at least 500 MB in size. Ensure that there is 4 MB of free space preceding the partition.
* Copy the following files to the **SD card**:
    * ***BOOT.BIN***
    * ***boot.scr***
    * ***image.ub***

## Prepare NFS Root
With **NFS Boot** it is possible to use several **Root File Systems** that can be selected in the bootloading process. ***[See The First Boot](#the-first-boot)***.

For example, to use a **PetaLinux root file system** created for this project with **XVC (Xilinx Virtual Cable)**, follow the steps below:
* Create a directory for the root file system with the following command:
    ```bash
    sudo mkdir /tftpboot/nfsroot/plnx_rootfs
    ```
* Extract the root file system with the following command:
    ```bash
    sudo tar -xfvp ~/<Project_Name>/images/linux/rootfs.tar.gz -C /tftpboot/nfsroot/plnx_rootfs
    ```
Or for example **ArchLinuxARM aarch64 root file system** can be used with the following steps:
* Download the latest Arch Linux ARM to the home directory:
    ```bash
    wget http://os.archlinuxarm.org/os/ArchLinuxARM-aarch64-latest.tar.gz
    ```
* Create a directory for the root file system with the following command:
    ```bash
    sudo mkdir /tftpboot/nfsroot/arch_rootfs
    ```
* Extract the root file system with the following command:
    ```bash
    sudo tar -xfvp ~/ArchLinuxARM-aarch64-latest.tar.gz -C /tftpboot/nfsroot/arch_rootfs
    ```

## The First Boot
After the SD card is prepared and the NFS root is set up, insert the SD card into the board and power it on. At the boot process hit any key to stop autoboot and enter the U-Boot CLI. U-Boot command line interface (CLI) will be displayed on the terminal. The U-Boot CLI can be used to set up the environment variables for the boot NFS root file system and other configurations.
### U-Boot CLI
To help using the U-Boot CLI, use the following command:
```u-boot
help
```
To set up the environment variables for the boot **PetaLinux root file system**, run the following commands in the U-Boot CLI:
```u-boot
setenv bootargs console=ttyPS0,115200 root=/dev/nfs nfsroot=<host_ip>:/tftpboot/nfsroot/plnx-rootfs,tcp,nfsvers=4 ip=dhcp rw
``` 
Save the environment variables with the following command:
```u-boot
saveenv
```
To verify that the environment variables has been set, use the following command:
```u-boot
printenv
```
After saving the environment variables, boot the device with the following command:
```u-boot
boot
```
Or reset the device with the following command:
```u-boot
reset
```

## Setup Petalinux
After the PetaLinux system boots up, the following steps can be taken to set up the PetaLinux environment.
### Configure .bashrc
Edit the **.bashrc** file:
```bash
sudo nano ~/.bashrc
```
Clear the file and add the following:
```text
# ~/.bashrc: executed by bash(1) for non-login shells.i
# see /usr/share/doc/bash/examples/startup-files (in the package bash-doc)
# for examples

# If not running interactively, don't do anything
case $- in
    *i*) ;;
      *) return;;
esac

# don't put duplicate lines or lines starting with space in the history.
# See bash(1) for more options
HISTCONTROL=ignoreboth

# append to the history file, don't overwrite it
shopt -s histappend

# for setting history length see HISTSIZE and HISTFILESIZE in bash(1)
HISTSIZE=1000
HISTFILESIZE=2000

# check the window size after each command and, if necessary,
# update the values of LINES and COLUMNS.
shopt -s checkwinsize

# If set, the pattern "**" used in a pathname expansion context will
# match all files and zero or more directories and subdirectories.
#shopt -s globstar

# make less more friendly for non-text input files, see lesspipe(1)
[ -x /usr/bin/lesspipe ] && eval "$(SHELL=/bin/sh lesspipe)"

# set variable identifying the chroot you work in (used in the prompt below)
if [ -z "${debian_chroot:-}" ] && [ -r /etc/debian_chroot ]; then
    debian_chroot=$(cat /etc/debian_chroot)
fi

# set a fancy prompt (non-color, unless we know we "want" color)
case "$TERM" in
    xterm-color|*-256color) color_prompt=yes;;
esac

# uncomment for a colored prompt, if the terminal has the capability; turned
# off by default to not distract the user: the focus in a terminal window
# should be on the output of commands, not on the prompt
#force_color_prompt=yes

if [ -n "$force_color_prompt" ]; then
    if [ -x /usr/bin/tput ] && tput setaf 1 >&/dev/null; then
	# We have color support; assume it's compliant with Ecma-48
	# (ISO/IEC-6429). (Lack of such support is extremely rare, and such
	# a case would tend to support setf rather than setaf.)
	color_prompt=yes
    else
	color_prompt=
    fi
fi

if [ "$color_prompt" = yes ]; then
    PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '
else
    PS1='${debian_chroot:+($debian_chroot)}\u@\h:\w\$ '
fi
unset color_prompt force_color_prompt

# If this is an xterm set the title to user@host:dir
case "$TERM" in
xterm*|rxvt*)
    PS1="\[\e]0;${debian_chroot:+($debian_chroot)}\u@\h: \w\a\]$PS1"
    ;;
*)
    ;;
esac

# enable color support of ls and also add handy aliases
if [ -x /usr/bin/dircolors ]; then
    test -r ~/.dircolors && eval "$(dircolors -b ~/.dircolors)" || eval "$(dircolors -b)"
    alias ls='ls --color=auto'
    #alias dir='dir --color=auto'
    #alias vdir='vdir --color=auto'

    alias grep='grep --color=auto'
    alias fgrep='fgrep --color=auto'
    alias egrep='egrep --color=auto'
fi

# colored GCC warnings and errors
#export GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'

# some more ls aliases
alias ll='ls -alF'
alias la='ls -A'
alias l='ls -CF'

# Add an "alert" alias for long running commands.  Use like so:
#   sleep 10; alert
alias alert='notify-send --urgency=low -i "$([ $? = 0 ] && echo terminal || echo error)" "$(history|tail -n1|sed -e '\''s/^\s*[0-9]\+\s*//;s/[;&|]\s*alert$//'\'')"'

# Alias definitions.
# You may want to put all your additions into a separate file like
# ~/.bash_aliases, instead of adding them here directly.
# See /usr/share/doc/bash-doc/examples in the bash-doc package.

if [ -f ~/.bash_aliases ]; then
    . ~/.bash_aliases
fi

# enable programmable completion features (you don't need to enable
# this, if it's already enabled in /etc/bash.bashrc and /etc/profile
# sources /etc/bash.bashrc).
if ! shopt -oq posix; then
  if [ -f /usr/share/bash-completion/bash_completion ]; then
    . /usr/share/bash-completion/bash_completion
  elif [ -f /etc/bash_completion ]; then
    . /etc/bash_completion
  fi
fi

# >>> conda initialize >>>
# !! Contents within this block are managed by 'conda init' !!
__conda_setup="$('/home/vtuser/miniforge3/bin/conda' 'shell.bash' 'hook' 2> /dev/null)"
if [ $? -eq 0 ]; then
    eval "$__conda_setup"
else
    if [ -f "/home/vtuser/miniforge3/etc/profile.d/conda.sh" ]; then
        . "/home/vtuser/miniforge3/etc/profile.d/conda.sh"
    else
        export PATH="/home/vtuser/miniforge3/bin:$PATH"
    fi
fi
```
### DNF Update
Update the package list:
```bash
sudo dnf update
```
Upgrade a package or packages on your system:
```bash 
sudo dnf upgrade
```
Install necessary packages:
```bash
sudo dnf install htop wget
```
After the installation is complete, ned to install **Conda** to more flexible environment management.
### Install Conda
Follow the instructions on the [Miniforge GitHub](https://github.com/conda-forge/miniforge) to install **Miniforge**:
```bash
wget "https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-$(uname)-$(uname -m).sh"
```
```bash
bash Miniforge3-$(uname)-$(uname -m).sh
```
And follow the instructions. After the installation is complete, the following message will be displayed:
```text
Do you wish to update your shell profile to automatically initialize conda?
This will activate conda on startup and change the command prompt when activated.
If you'd prefer that conda's base environment not be activated on startup,
   run the following command when conda is activated:
```
```bash
conda config --set auto_activate_base False
```
You can undo this by running:
```bash
`conda init --reverse $SHELL`? [yes|no]
```
To activate conda's base environment in your current shell session:
```bash
eval "$(/home/vtuser/miniforge3/bin/conda shell.YOUR_SHELL_NAME hook)"
```
* **YOUR_SHELL_NAME** - The name of your shell (e.g., **sh**, **bash**)
To install conda's shell functions for easier access, first activate, then:
```bash
conda init
```
### Update conda in base environment
```bash
conda update --name base conda
```
### Create Conda Environment
Create a new conda environment with the following command:
```bash
conda create --name <env_name>
```
### Activate Conda Environment
Activate the conda environment with the following command:
```bash
conda activate <env_name>
```
### Deactivate Conda Environment
Deactivate the conda environment with the following command:
```bash
conda deactivate
```
### Install necessary packages
To install necessary packages, run the following command:
```bash
conda install <package_name>
```
For example, to install **Python3**, **GCC**, **nano**, **htop**, run the following command:
```bash
conda install python gcc nano htop
```
### How to use Conda
To use conda, follow the instructions on the [Conda Documentation](https://docs.conda.io/projects/conda/en/stable).

## FPGA Manager
### Create directory for bitstreams and copy the bitstream to the directory:
```bash
mkdir Bitstreams
```
### Configure bitstream:
```bash
sudo fpgautil -b Bitstreams/<bitstream_name>.bit
```
### If the bitstream is configured correctly, the following message will be displayed:
```text
Time taken to load BIN is 244.000000 Milli Seconds
BIN FILE loaded through FPGA manager successfully
```

## Hardware Manager
### Hardware Targets
The Hardware Manager is used to manage the hardware targets in the Vivado Design Suite. The Hardware Manager allows you to program and debug the hardware design on the FPGA device.
Open Hardware Manager --> Open Target --> Open Hardware Target --> Hardware Server Settings --> Connect to: Local Server --> Select Hardware Target --> Add Xilinx Virtual Cable (XVC) --> Host name: <NFS_Server_IP> --> Port: 2542 --> OK --> Finish
![Hardware Target](Images/hw_target.jpg "Hardware Target")
### Hardware ILA
The Hardware Integrated Logic Analyzer (ILA) is a debug core that allows you to monitor internal signals in design in real-time. The ILA core is used to capture signals in the design and display them in the Vivado Logic Analyzer. Configure the ILA according to the block design example.
![Hardware ILA](Images/ila.jpg "Hardware ILA")

## Example of Block Design
Block Design Example used for testing the PetaLinux system with NFS Root and XVC integration
![Block design example](Images/bd.jpg "Block design example")
This simle **Block Design** includes following blocks:
* **Zynq UltraScale+ MPSoC**
* **AXI Interconnect**
* **Processor System Reset**
* **AXI BRAM Controllers** with **Block Memory Generator** 
* **Debug Bridge** is used for remote debugging through **XVC** and configured as **JTAG to BSCAN**
* **System ILA** is configured to capture the following interfaces:
    * ***zynq_ultra_ps_e_0_M_AXI_HPM0_LPD***
    * ***axi_interconnect_0_M00_AXI***
    * ***axi_interconnect_0_M01_AXI***
    * ***axi_interconnect_0_M02_AXI***
    * ***axi_interconnect_0_M03_AXI***
 
## Glossary
| Abbreviation  | Definition                            |
| ------------- | ------------------------------------- |
| **A**         |                                       |
| **ALARM**     | Arch Linux ARM                        |
| **AXI**       | Advanced eXtensible Interface         |
| **B**         |                                       |
| **BASH**      | Bourne Again Shell                    |
| **BB**        | BitBake                               |
| **BD**        | Block Design                          |
| **BRAM**      | Block RAM                             |
| **BSCAN**     | Boundary Scan                         |
| **C**         |                                       |
| **CLI**       | Command Line Interface                |
| **F**         |                                       |
| **FPGA**      | Field-Programmable Gate Array         |
| **FSBL**      | First Stage Boot Loader               |
| **I**         |                                       |
| **ILA**       | Integrated Logic Analyzer             |
| **IP**        | Intellectual Property                 |
| **J**         |                                       |
| **JTAG**      | Joint Test Action Group               |
| **M**         |                                       |
| **MPSoC**     | Multi-Processor System on Chip        |
| **N**         |                                       |
| **NFS**       | Network File System                   |
| **P**         |                                       |
| **PL**        | Programmable Logic                    |
| **PLNX**      | PetaLinux                             |
| **PMUFW**     | Platform Management Unit Firmware     |
| **PS**        | Processing System                     |
| **R**         |                                       |
| **RPT**       | Remote Peripheral Testing             |
| **S**         |                                       |
| **SD**        | Secure Digital                        |
| **SDHCI**     | Secure Digital Host Controller        |
| **SH**        | Shell                                 |
| **SHELL**     | Command Line Interpreter              |
| **SoC**       | System on Chip                        |
| **T**         |                                       |
| **TFTP**      | Trivial File Transfer Protocol        |
| **U**         |                                       |
| **U-Boo**     | Universal Boot                        |
| **V**         |                                       |
| **VM**        | Virtual Machine                       |
| **W**         |                                       |
| **WP**        | Write Protect                         |
| **X**         |                                       |
| **XDC**       | Xilinx Design Constraints             |
| **XLNX**      | Xilinx                                |
| **XSA**       | Xilinx Support Archive                |
| **XVC**       | Xilinx Virtual Cable                  |