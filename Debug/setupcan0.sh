#!/bin/bash
sudo ip link set can0 type can bitrate 125000
sudo ip link set up can0
echo "can0 should be up..."
gnome-terminal -- /home/hugo/eclipse-workspace/CAN-Acquisition-Module_Server/Debug/synctoonedrive.sh
/home/hugo/eclipse-workspace/CAN-Acquisition-Module_Server/Debug/CAN-Acquisition-Module_Server

