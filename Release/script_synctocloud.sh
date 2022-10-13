#!/bin/bash  


while true  
do  
  echo "Synching to Onedrive now..."
  #rclone copy /home/hugo/Documents/AcqData onedrive:/AcqData/
  rclone copy $1 $2:$3
  #sleep 300
  sleep $4
done

