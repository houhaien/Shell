#!/bin/bash

NowDate=$(date "+%Y-%m-%d__%H:%M:%S")
#echo $NowDate
UserNum=` cat /etc/passwd | awk -F ":" '{if($3 >= 1000 && $3 != 65534){printf("%s\n", $1)}}' | wc -l `
#echo $UserNum

UserName=`last | cut -f -1 -d " " | uniq | head -n 3`

#echo $UserName

RootName=(`cat /etc/group | grep sudo | cut -d : -f 4`)
#echo $RootName


NameIpTty=(`w | tail -n +3 | awk '{print","$1"_"$3"_"$2}' | tr -d '\n' | cut -d ',' -f 2-`)

#echo $NameIpTty

echo $NowDate $UserNum [$UserName] [$RootName] [$NameIpTty]

