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
 7. [PetaLinux Project](#petalinux-project)
     * [Create a PetaLinux Project](#create-a-petalinux-project)
     * [Device Tree Configuration](#device-tree-configuration)
     * [Hardware Configuration](#hardware-configuration)
     * [Root File System Configuration](#root-file-system-configuration)
     * [Kernel configuration](#kernel-configuration)
     * [Build PetaLinux Project](#build-petalinux-project)
     * [Packaging a Petalinux Project](#packaging-a-petalinux-project)
 8. [Xilinx Virtual Cable (XVC)](#xilinx-virtual-cable-xvc)
     * [Overview XVC](#overview-xvc)
     * [Hardware Configuration](#hardware-configuration)
     * [Edit Petalinux Project](#edit-petalinux-project)
     * [Create Kernel Module (xvc-driver)](#create-kernel-module-xvc-driver)
     * [Create Application (xvcserver)](#create-application-xvcserver)
     * [Start XVC Driver](#start-xvc-driver)
     * [Start XVC Server](#start-xvc-server)
 9. [Remote Peripheral Testing (RPT)](#remote-peripheral-testing-rpt)
     * [Overview RPT](#overview-rpt)
     * [Hardware Configuration](#hardware-configuration)
     * [Edit Petalinux Project](#edit-petalinux-project)
     * [Create Application (rptserver)](#create-application-rptserver)
     * [Start RPT Server](#start-rpt-server)
10. [Prepare SD Card](#prepare-sd-card)
11. [Prepare NFS Root](#prepare-nfs-root)
12. [The First Boot](#the-first-boot)
     * [U-Boot CLI](#u-boot-cli)
13. [Setup Petalinux](#setup-petalinux)
     * [Configure .bashrc](#configure-bashrc)
     * [DNF Update](#dnf-update)
     * [Install Conda](#install-conda)
     * [Create Conda Environment](#create-conda-environment)
     * [Install necessary packages](#install-necessary-packages)
14. [Remote development with VS Code](#remote-development-with-vs-code)     
15. [FPGA Manager](#fpga-manager)
16. [Hardware Manager](#hardware-manager)
     * [Hardware Targets](#hardware-targets)
     * [Hardware ILA](#hardware-ila)


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
The project aims to configure a PetaLinux system with an NFS root file system and integrate XVC for remote debugging capabilities of FPGA or SoC devices. 

## Create Ubuntu 22.04.2 Virtual Machine (VM)
### Minimum requirements to run PetaLinux Tools 2023.2:
* 8 GB RAM (recommended minimum for AMD tools)
* 2 GHz CPU clock or equivalent (minimum of eight cores)
* 100 GB free disk space
* Ubuntu 22.04.2 LTS (64-bit) (latest version unsupported)
* Internet connection
* System shell /bin/bash or /bin/sh

### Install the required packages
Before installing the PetaLinux Tools, ensure that the required packages are installed on the Ubuntu 22.04.2 VM. Change the system shell to /bin/bash or /bin/sh if necessary.

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
### Add to BD Debug Bridge
For this project, the Debug Bridge type is configured as AXI to BSCAN. This bridge type is intended for designs that use Xilinx Virtual Cable (XVC) to remotely debug on FPGA or SoC device through Ethernet or other interfaces without the need for JTAG cable. In this mode, the Debug Bridge receives XVC Commands via AXI4-Lite interface.
![Debug bridge configuration](Images/debug_bridge.jpg "Debug bridge configuration")
Add the **Debug Bridge** to the Block Design, as shown in the ***Block Design Example***.
### Add to BD System ILA
Configure the System ILA to capture the interfaces of interest. For this project, the System ILA is configured to capture the following interfaces (see example of block design above).
![System ILA configuration](Images/system_ila.jpg "System ILA configuration")
Add the ILA Core to the Block Design, as shown in the Block ***Design Example***.
### Block Design Example
![Block design example](Images/bd.jpg "Block design example")


## PetaLinux Project
### Create a PetaLinux Project
### Device Tree Configuration
### Hardware Configuration
### Root File System Configuration
### Kernel configuration
### Build PetaLinux Project
### Packaging a Petalinux Project

## Xilinx Virtual Cable (XVC)
### Overview XVC
### Hardware Configuration
### Edit Petalinux Project
### Create Kernel Module (xvc-driver)
### Create Application (xvcserver)
### Start XVC Driver
### Start XVC Server

## Remote Peripheral Testing (RPT)
### Overview RPT
### Hardware Configuration
### Edit Petalinux Project
### Create Application (rptserver)
### Start RPT Server

## Prepare SD Card

## Prepare NFS Root

## The First Boot
### U-Boot CLI

## Setup Petalinux
### Configure .bashrc
### DNF Update
### Install Conda
### Create Conda Environment
### Install necessary packages

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
### Hardware ILA
