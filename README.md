# PetaLinux Flexible NFS Root with XVC

1. [Introduction](#introduction)
2. [Overview](#overview)
3. [Prerequisites](#prerequisites)
    * [TFTP Server](#tftp-server)
        * [Install TFTP Server](#install-tftp-server)
        * [Create TFTP Root Directory](#create-tftp-root-directory)
            * [Change the TFTP Server Directory’s Permissions](#change-the-tftp-server-directorys-permissions)
        * [Configure TFTP Server](#configure-tftp-server)
        * [Restart TFTP Server](#restart-tftp-server)
        * [Verify TFTP Server](#verify-tftp-server)
        * [Start TFTP Server](#start-tftp-server)
        * [Start TFTP Server on Boot](#start-tftp-server-on-boot)
    * [NFS Server](#nfs-server)
        * [Install NFS Server](#install-nfs-server)
        * [Create NFS Root Directory](#create-nfs-root-directory)
            * [Set Permissions](#set-permissions)
        * [Export NFS Root (***important***)](#export-nfs-root-important)
        * [Restart NFS Server](#restart-nfs-server)
        * [Add rules to the firewall](#add-rules-to-the-firewall)
            * [Enable the firewall](#enable-the-firewall)
            * [Firewall status](#firewall-status)
        * [Verify NFS Server](#verify-nfs-server)
    * [NFS Client](#nfs-client)
        * [Install NFS Client](#install-nfs-client)
        * [Create NFS Mount Point](#create-nfs-mount-point)
        * [Mount NFS Share](#mount-nfs-share)        
    * [Prepare the Hardware](#prepare-the-hardware)
        * [Block Design Example](#block-design-example)
        * [Debug Bridge Configuration](#debug-bridge-configuration)
        * [System ILA Configuration](#system-ila-configuration)
4. [PetaLinux Project](#petalinux-project)
    * [Requirements](#requirements)
    * [Create a PetaLinux Project](#create-a-petalinux-project)
        * [Device Tree Configuration](#device-tree-configuration)
    * [PetaLinux Configuration](#petalinux-configuration)
        * [Kernel configuration](#kernel-configuration)
    * [Create Kernel Module](#create-kernel-module)
        * [Setup XVC Driver](#setup-xvc-driver)
    * [Create Application](#create-application)
        * [Setup XVC Server](#setup-xvc-server)
        * [Setup RPT Server (optional)](#setup-rpt-server-optional)
5. [Prepare SD Card](#prepare-sd-card)
6. [Prepare NFS Root](#prepare-nfs-root)
7. [First Boot](#first-boot)
    * [U-Boot CLI](#u-boot-cli)
        * [Setup Environment Variables](#setup-environment-variables)
8. [Starting the Software](#starting-the-software)
    * [Start XVC Driver](#start-xvc-driver)
    * [Start XVC Server](#start-xvc-server)
    * [Start RPT Server (optional)](#start-rpt-server-optional)

9. [Hardware Manager](#hardware-manager)
    * [Hardware Targets](#hardware-targets)
    * [System ILA](#system-ila)    

## Introduction

## Overview

## Prerequisites
### TFTP Server
TFTP (Trivial File Transfer Protocol) is a simple protocol used for transferring files over a network. A TFTP server is a program or service that implements the TFTP protocol, allowing devices to upload or download files to or from it.

#### Install TFTP Server
To install the TFTP server, run the following commands:
```bash
sudo apt update
sudo apt install tftpd-hpa
```

#### Create TFTP Root Directory
Create a directory for the TFTP server with the following command:
```bash
sudo mkdir /tftpboot
```

#### Change the TFTP Server Directory’s Permissions
Change the directory’s permissions with the following commands:
```bash
sudo chown nobody:nogroup /tftpboot
sudo chmod 777 /tftpboot
```

#### Configure TFTP Server
Edit the TFTP server configuration file with the following command:
```bash
sudo nano /etc/default/tftpd-hpa
```
If necessary, change the following lines:
```bash
TFTP_USERNAME="tftp"
TFTP_DIRECTORY="/tftpboot"
TFTP_ADDRESS="0.0.0.0:69"
TFTP_OPTIONS="--secure"
```

#### Restart TFTP Server
After changing the configuration file, restart the TFTP server with the following command:
```bash
sudo systemctl restart tftpd-hpa
```

#### Verify TFTP Server
To verify that the TFTP server is running, run the following command:
```bash
sudo systemctl status tftpd-hpa
```

#### Start TFTP Server
If the TFTP server is not running, start it with the following command:
```bash
sudo systemctl start tftpd-hpa
```

#### Start TFTP Server on Boot
To start the TFTP server on boot, run the following command:
```bash
sudo systemctl enable tftpd-hpa
```

### NFS Server
Network File System (NFS) is a distributed file system protocol allowing a user on a client computer to access files over a network in a manner similar to how local storage is accessed. An NFS server is a system that hosts shared directories, making them available to clients on the network.

#### Install NFS Server
To install the NFS server, run the following command:
```bash
sudo apt update
sudo apt install nfs-kernel-server
```

#### Create NFS Root Directory
Create a directory for the NFS server with the following command:
```bash
sudo mkdir /tftpboot/nfsroot
```

#### Set Permissions
Change the directory’s permissions with the following commands:
```bash
sudo chown nobody:nogroup /tftpboot/nfsroot
sudo chmod 777 /tftpboot/nfsroot
```

#### Export NFS Root (***important***)
Edit the NFS server configuration file with the following command:
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

Save the file and exit (Ctrl + s, Ctrl + x) the editor. After changing the configuration file, make the export with the following command:
```bash
sudo exportfs -a
```

#### Restart NFS Server
After changing the configuration file, restart the NFS server with the following command:
```bash
sudo systemctl restart nfs-kernel-server
```

#### Add rules to the firewall
To add rules to the firewall, run the following commands:
```bash
sudo ufw allow from 192.168.1.0/24 to any port nfs
```

#### Enable the firewall
To enable the firewall, run the following command:
```bash
sudo ufw enable
```

#### Firewall status
To check the firewall status, run the following command:
```bash
sudo ufw status
```

#### Verify NFS Server
To verify that the NFS server is running, run the following command:
```bash
sudo systemctl status nfs-kernel-server
sudo showmount -e
```

### NFS Client
To access files shared by an NFS server from a client machine, you need to set up the NFS client software and mount the shared directories.

#### Install NFS Client
To install the NFS client, run the following command:
```bash
sudo apt update
sudo apt install nfs-common
```

#### Create NFS Mount Point
Create a directory for the NFS mount point with the following command:
```bash
sudo mkdir ~/nfsroot_client
```

#### Mount NFS Share
To mount the NFS share, run the following command:
```bash
sudo mount <NFS_Server_IP>:/tftpboot/nfsroot ~/nfsroot_client
```

### Prepare the Hardware
Add to your hardware design the following IP cores:
* **Debug Bridge**
* **System ILA**
#### Block Design Example
<!-- ![Block design example](Images/bd.jpg "Block design example") -->

#### Debug Bridge Configuration
For this project, the Debug Bridge type is configured as AXI to BSCAN. This bridge type is intended for designs that use Xilinx Virtual Cable (XVC) to remotely debug on FPGA or SoC device through Ethernet or other interfaces without the need for JTAG cable. In this mode, the Debug Bridge receives XVC Commands via AXI4-Lite interface.    
<!-- ![Debug bridge configuration](Images/debug_bridge.jpg "Debug bridge configuration") -->

#### System ILA Configuration
Configure the System ILA to capture the interfaces of interest. For this project, the System ILA is configured to capture the following interfaces (see example of block design above):
<!-- ![System ILA configuration](Images/system_ila.jpg "System ILA configuration") -->
## PetaLinux Project
### Requirements
* [PetaLinux Tools Documentation: Reference Guide (UG1144)
](https://docs.amd.com/r/en-US/ug1144-petalinux-tools-reference-guide)
* [Ubuntu 22.04.2 LTS (virtual machine is preferred)](https://old-releases.ubuntu.com/releases/20.04.2/)
* [PetaLinux Tools 2023.2 (installed according to the documentation)](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-design-tools.html)

### Create a PetaLinux Project
An empty project is created with the following command:
```bash
petalinux-create --type project --template zynqMP --name <project name>
cd <project name>
```

#### Device Tree Configuration
Add the following lines to the device tree file:
```dts
/include/ "system-conf.dtsi"
/ {
    chosen {
        bootargs = "console=ttyPS0,115200 root=/dev/nfs nfsroot=<NFS Server IP>:/tftpboot/nfsroot/plnx_rootfs,tcp,nfsvers=4 ip=dhcp rw";
	}:
};

&sdhci0 {
       no-1-8-v;
       disable-wp;
};

&sdhci1 {
       no-1-8-v;
       disable-wp;
};

&debug_bridge_0 {
        compatible = "xlnx,xvc";
};
```
* **NFS Server IP** - Change this to the IP address of the NFS server
* **debug_bridge_0** - Compatibility string for the device driver

### PetaLinux Configuration
After creating the project, configure the PetaLinux project with the following command:
```bash
petalinux-config --get-hw-description ~/<file name>.xsa
```
After the XSA file has been imported successfully, the PetaLinux system configuration is opened. The following settings are changed:
```text
→ Subsystem AUTO Hardware Settings  --->
	Ethernet Settings  --->
		[*] Randomise MAC address

→ FPGA Manager --->
	[*] Fpga Manager
	
→ Image Packaging Configuration --->
	Root filesystem type (NFS) --->
		(X) NFS
	(/tftpboot/nfsroot) Location of NFS root directory --->
		/tftpboot/nfsroot
	[*] Copy final images to tftpboot
	(/tftpboot) tftpboot directory --->
	    /tftpboot	
```

#### Kernel configuration
Check kernel Configuration in accordance with PetaLinux Tools Documentation: Reference Guide (UG1144) "Configuring NFS Boot" section. 

Use the following command to open the kernel configuration:
```bash
petalinux-config -c kernel
```

### Create Kernel Module
#### Setup XVC Driver
Create a kernel module for the XVC driver with the following command:
* **Avoid using capital letters and underscore '_' when creating module name**
```bash
petalinux-create -t modules --name xvc-driver --enable
```
Copy following files: **xvc_driver_base.c**, **xvc_driver.c**, **xvc_driver.h**, **xvc_ioctl.h**, **xvc_user_config.h** into recipe to build the kernel module in the **project-spec/meta-user/recipes-modules/xvc-driver/files** directory. Files can be found in the [XVC_ZynqMP/xvc-driver](XVC_ZynqMP/xvc-driver/files) directory or copy and replace entire **xvc-driver** folder.

Change the SRC_URI BitBake variable in the **project-spec/meta-user/recipes-modules/xvc-driver/xvc-driver.bb** recipe file to:
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
    petalinux-build -c xvc-driver
```
Also need to rebuild PetaLinux bootable images run this command:
```bash
    petalinux-build -c rootfs
```

### Create Application
#### Setup XVC Server
* **Avoid using capital letters and underscore '_' when creating application name**
Create XVC server application with the following command:
```bash
petalinux-create -t apps --name xvcserver --enable
```
Copy the following files: **xvcserver.c**, **xvc_ioctl.h** and **Makefile** into the recipe to build the XVC server application in the **project-spec/meta-user/recipes-apps/xvcserver/files** directory. Files can be found in the [XVC_ZynqMP/xvcserver](XVC_ZynqMP/xvcserver/files) directory or copy and replace entire **xvcserver** folder.

Change the SRC_URI BitBake variable in the **project-spec/meta-user/recipes-apps/xvcserver/xvcserver.bb** recipe file (in folder [XVC_ZynqMP/xvcserver](XVC_ZynqMP/xvcserver/files) all files already modified) to:
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
Check if the xvcserver application is enabled in the configuration in:
```bash
    user packages ---> 
        [*] xvcserver
```
To build xvcserver application, run the following command:
```bash
petalinux-build -c xvcserver
```

#### Setup RPT Server (optional)
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
Also need to rebuild PetaLinux bootable images run this command:
```bash
petalinux-build -c rootfs
```
Then, build the PetaLinux project with the following command:
```bash
petalinux-build
```
### Packaging a Petalinux Project
After the project is built, the project is packaged with the following command:
```bash
petalinux-package --boot --format BIN --fsbl images/linux/zynqmp_fsbl.elf --u-boot images/linux/u-boot.elf --pmufw images/linux/pmufw.elf --fpga images/linux/system.bit --force
```



## Prepare SD Card

## Prepare NFS Root

## First Boot
### U-Boot CLI
#### Setup Environment Variables

## Starting the Software
### Start XVC Driver
### Start XVC Server
### Start RPT Server (optional)

## Hardware Manager
### Hardware Targets
### System ILA

