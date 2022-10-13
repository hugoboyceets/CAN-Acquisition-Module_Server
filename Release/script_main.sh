#!/bin/bash
#
# This is the bash script to start the CANbus data acquisition server
#
# This script will start the data acquisition program that polls the sensors over CANbus
# and saves the results to disk as a .csv file. This script will then also start an other script
# which is responsible for cloning the local .csv files to the cloud using rclone. 
#
# The scripts assume that rclone is already installed and configured with the desired remote.
# This link shows how to install rclone: https://rclone.org/downloads/ or on Ubuntu open a terminal and type "sudo apt install rclone"
# this link shows how to set up a Microsoft onedrive remote after rclone is installed: https://rclone.org/onedrive/
#

# Firstly, let's define the CANbus adapter properties/configuration
INTERFACE="can0"
BITRATE="125000" #kbits/second. This should match what's in the sensor module firmware. No autobaud for now...

# Secondly, let's define where we want the program to put the datalogging files, and how often to take a sample
LOCAL_DATALOGGING_PATH="/home/hugo/Documents/AcqData"
DATALOGGING_SAMPLE_PERIOD="60" #seconds. Time interval between data samples from the sensors

# Thirdly, let's define where, and how often the files are copied to the cloud
REMOTE_NAME="onedrive" # This is the name of the (previously configured) remote cloud we want rclone to put the files on.
REMOTE_PATH="/AcqData/" # This is where we want rclone to put the files on the remote cloud.
REMOTE_UPLOAD_PERIOD="300" #seconds. This is the amount of time between each uploads to the remote cloud.



# Boilerplate to change the directory of execution to the directory where this script is
export DIRNAME="$(dirname "$(readlink -f "$0")")"
cd "$DIRNAME"

# Then, we can go on with the actual script.

# To begin, the CAN interface is set up
sudo ip link set $INTERFACE type can bitrate $BITRATE
sudo ip link set up $INTERFACE

# Then, start the sript that syncs to the cloud in a new terminal
gnome-terminal -- ./script_synctocloud.sh $LOCAL_DATALOGGING_PATH $REMOTE_NAME $REMOTE_PATH $REMOTE_UPLOAD_PERIOD

# Finally, start the datalogging program
./CAN-Acquisition-Module_Server $INTERFACE $DATALOGGING_SAMPLE_PERIOD $LOCAL_DATALOGGING_PATH 

