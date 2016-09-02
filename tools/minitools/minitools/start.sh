#!/bin/sh
LSB=`lsb_release -i | awk '{print $3}'`
if [ "$LSB" = "Ubuntu" ]; then
	X64=`uname -a | grep x86_64`
	if [ ! -z "$X64" ]; then
		echo "MiniTools_x64"
		sudo ./MiniTools_x64 1>/dev/null 2>/dev/null
		exit 0
	fi
fi
echo "MiniTools_x86_static"
sudo ./MiniTools 1>/dev/null 2>/dev/null
