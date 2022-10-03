#!/bin/sh  
while true  
do  
  echo "Synching to Onedrive now..."
  /home/hugo/rclone-v1.59.1-linux-amd64/rclone copy /home/hugo/Documents/AcqData onedrive:/AcqData/
  sleep 300  
done

