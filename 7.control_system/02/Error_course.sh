#!/bin/bash

dateinfo=$(date "+%Y-%m-%d__%H:%M:%S")
#echo $dateinfo


CpuEr=(`ps -aux -h | sort -nrk 3 | awk '{if ($3 > 2.0 || $4 > 2.0) print $2}'`)
#echo ${CpuEr[@]}
if [[ $CpuEr != " " ]];then
    sleep 5
    for i in $CpuEr;do
        #搜索某个进程号 ,并列出全部信息 ps -p pid u
        eval $(ps -p $i -u | tail -n 1 | awk -v pid=0 -v name="" -v usr="" -v cpunum=0 -v memnum=0 '{if($3> 2.0 || $4>2.0) printf ("name=%s; usr=%s; pid=%d; cpunum=%.2f; memnum=%.2f;",$11, $1, $2, $3, $4)}')
        echo "$dateinfo $name $pid $usr $cpunum% $memnum%"
    done

fi
