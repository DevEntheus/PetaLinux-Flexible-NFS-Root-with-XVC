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
    * [Create a PetaLinux Project](#create-a-petalinux-project)
        * [Device Tree Configuration](#device-tree-configuration)
    * [PetaLinux Configuration](#petalinux-configuration)
        * [Root filesystem type](#root-filesystem-type)
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
<hr>

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
___
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
___
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
___
Add to your hardware design the following IP cores:
* **Debug Bridge**
* **System ILA**
#### Block Design Example
![Block design example](Images/bd.jpg "Block design example")
#### Debug Bridge Configuration
For this project, the Debug Bridge type is configured as AXI to BSCAN. This bridge type is intended for designs that use Xilinx Virtual Cable (XVC) to remotely debug on FPGA or SoC device through Ethernet or other interfaces without the need for JTAG cable. In this mode, the Debug Bridge receives XVC Commands via AXI4-Lite interface.    
![Debug bridge configuration](Images/debug_bridge.jpg "Debug bridge configuration")
#### System ILA Configuration
Configure the System ILA to capture the interfaces of interest. For this project, the System ILA is configured to capture the following interfaces (see example of block design above):
![System ILA configuration](Images/system_ila.jpg "System ILA configuration")
## PetaLinux Project
### Create a PetaLinux Project
___
#### Device Tree Configuration
### PetaLinux Configuration
___
#### Root filesystem type
#### Kernel configuration
### Create Kernel Module
___
#### Setup XVC Driver
### Create Application
___
#### Setup XVC Server
#### Setup RPT Server (optional)

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

