#!/bin/bash
sudo ip link set can0 type can bitrate 125000
sudo ip link set up can0
read -p "Script Done. Press enter to continue..."

