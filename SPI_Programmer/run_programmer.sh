#!/bin/bash
#echo "sudo rmmod ftdi_sio"
lsmod | grep ftdi_sio
if [ "$?" == "0" ]; then
	RET=1
	while [ "$RET" == "1" ]; do
		sudo rmmod ftdi_sio
		RET=$?
		echo "Waiting for ftdi_sio ready to be removed ..."
		sleep 1
	done
fi
COMMAND=$1
FILE=$2
echo "Running"
sudo LD_LIBRARY_PATH=libs bin/Debug/SPI_Programmer $COMMAND $FILE

