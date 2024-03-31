#!/bin/bash

#PetaLinux environment setup script
#original version by Tony McDowell (tmcdowe@xilinx.com)
#updated version by Sandeep Gundlupet Raju (sandeep.gundlupet-raju@xilinx.com)

# Enable debug=1 mode -- this disables actual changes on the host machine using dry-run options.
# Disable debbug=0 to proceed with installation
debug=0;

#get OS pretty name
osPrettyName=`cat /etc/os-release | grep PRETTY_NAME | sed 's/.*="\(.*\)"/\1/'`;
centosVersion=`cat /etc/centos-release | sed 's/[^0-9.]*\([0-9.]*\).*/\1/'`;
osKernelVer=`uname -r`

echo "***************************************************************";
echo "PetaLinux Environment Setup Tool";
echo "Running on $osPrettyName ($osKernelVer)";
echo "***************************************************************";
#print the debug message header
if [ $debug -eq 1 ]; then echo "***DEBUG MODE ON!***"; fi; 
echo " "

echo -n "NOTE: Check for superuser..."
#get the actual user
if [ $SUDO_USER ]; then actualUser=$SUDO_USER; else actualUser=`whoami`; fi
#get the effective user
currentUser=`whoami`
if [ $currentUser != "root" ]; then echo "FAILED! \r\n Please re-run this script as sudo"; exit 1; else echo "SUCCESS! (from "$actualUser")"; fi;

#determine the host OS from the pretty_name
if [[ $(echo $osPrettyName | grep buntu) ]]; then
	hostOS="ubuntu";
	#echo "Running on Ubuntu";
elif [[ $(echo $osPrettyName | grep CentOS) ]]; then
	hostOS="centos";
	echo "Running on CentOS version $centosVersion";
elif [[ $(echo $osPrettyName | grep "Red Hat") ]]; then
	hostOS="rhel";
	#echo "Running on Red Hat";
else
	echo "ERROR: Cannot determine host operating system!"
	echo "This script is only supported on Ubuntu, CentOS, and RHEL Linux distribution!"
	exit 1;	
fi;

## declare the package lists
debPackages=(iproute2 gawk python3 build-essential gcc git make net-tools libncurses5 libtinfo5 libncurses5-dev libncursesw5-dev tftpd zlib1g-dev libssl-dev flex bison libselinux1 gnupg wget git-core diffstat chrpath socat xterm autoconf libtool tar unzip texinfo zlib1g-dev gcc-multilib automake zlib1g:i386 screen pax gzip cpio python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev);

rhelPackages=(net-tools gawk make wget tar bzip2 gzip python3 unzip perl patch diffutils diffstat git cpp gcc gcc-c++ glibc-devel texinfo chrpath socat perl-Data-Dumper perl-Text-ParseWords perl-Thread-Queue python3-pip python3-GitPython python3-jinja2 python3-pexpect xz which SDL-devel xterm autoconf libtool.x86_64 zlib-devel automake glib2-devel zlib ncurses-devel openssl-devel dos2unix flex bison glibc.i686 glibc.x86_64 screen pax glibc-devel.i686 compat-libstdc++-33.i686 libstdc++.i686 libstdc++.x86_64);

if [ $hostOS == "ubuntu" ]; then
	packageList=(${debPackages[@]});
elif [ $hostOS == "rhel" ]; then
	packageList=(${rhelPackages[@]});
elif [ $hostOS == "centos" ]; then
	packageList=(${rhelPackages[@]});
fi;

#update shell on UBUNTU only
#place this portion near the start of the script so that it runs before sudo expires if package installation takes a long time
if [ $hostOS == "ubuntu" ]; then
	echo -n "NOTE: Checking for DASH shell as default (Ubuntu only)...";
	if echo `echo $0` | grep 'dash'; then
		echo "FOUND!";
		echo -n "NOTE: Changing default shell to from DASH to BASH...";
		export DEBIAN_FRONTEND=noninteractive;
		export DEBCONF_NONINTERACTIVE_SEEN=true;

		echo "dash dash/sh boolean false" | debconf-set-selections;
		dpkg-reconfigure dash;

		unset DEBIAN_FRONTEND;
		unset DEBCONF_NONINTERACTIVE_SEEN;
		echo "DONE!";
		echo "INFO: You must log out of this shell and back in for change to take effect";
	else
		echo "NOT FOUND!";
	fi;
fi;

#start building the package installation command line
if [ $hostOS == "ubuntu" ]; then
	packageCommand="apt";
elif [ $hostOS == "rhel" ]; then
	packageCommand="yum";
elif [ $hostOS == "centos" ]; then
	packageCommand="yum";	
fi;

#DEBIAN/UBUNTU ONLY -- check if dpkg repo is set up for 32-bit packages
if [ $hostOS == "ubuntu" ]; then
	echo -n "NOTE: Check for x86 package access..."
	foreignArchitecture=`dpkg --print-foreign-architectures | grep i386`;
	if [ $foreignArchitecture == "i386" ]; then 
		echo "FOUND!"; 
	else 
		echo "NOT FOUND! Now adding i386 foreign archiecture to dpkg";
		sudo dpkg --add-architexture i386; 
	fi;
fi;

#make sure the package lists are up-to-date
echo "NOTE: Updating the package lists...";
if [ $hostOS == "ubuntu" ]; then
	sudo $packageCommand update;
elif [ $hostOS == "rhel" ]; then
	sudo $packageCommand makecache;
elif [ $hostOS == "centos" ]; then
	sudo $packageCommand makecache;
fi;

#install the packages
for package in "${packageList[@]}"; do
	echo "######";
	echo "NOTE: Processing package: "$package;
	echo -n "NOTE: Checking to see if package is already installed..."
	installPackage=0;
	
	if [ $hostOS == "ubuntu" ]; then
		if [[ $($packageCommand -qq list $package | grep installed) ]]; then
			echo "INSTALLED! Skipping."
		else
			echo "NOT INSTALLED!";
			echo "Starting installation of package $package";
			packageInstallCommand="$packageCommand install";
			if [ $debug -eq 1 ]; then 
				packageInstallCommand+=" --dry-run";
			else
				packageInstallCommand+=" -y";
			fi;
			sudo $packageInstallCommand $package;
		fi;
	elif [ $hostOS == "rhel" ] || [ $hostOS == "centos" ]; then
		if [[ $($packageCommand list installed | grep $package) ]]; then
			echo "INSTALLED! Skipping."
		else
			echo "NOT INSTALLED!";
			echo "Starting installation of package $package";
			packageInstallCommand="$packageCommand install";
			if [ $debug -eq 1 ]; then 
				packageInstallCommand+=" --assumeno";
			else
				packageInstallCommand+=" -y";
			fi;
			sudo $packageInstallCommand $package;
		fi;
		
	fi;	
	echo "Package installation complete for package $package";
	echo -e "######\n";

done;

# For CentOS/RHEL version 7 install GitPython jinja2 using pip3 commands
if [ $hostOS == "rhel" ] || [ $hostOS == "centos" ] && [ $debug -eq 0 ]; then
	echo "NOTE: Installing GitPython jinja2 using pip3 commands";
	pip3 install GitPython jinja2;
fi;

echo "INFO: Environment setup complete!";
