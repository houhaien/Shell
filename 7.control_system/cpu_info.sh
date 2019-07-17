#!/bin/bash
dateinfo=$(date "+%Y-%m-%d__%H:%M:%S")
#echo $dateinfo

load=`cat /proc/loadavg | cut -d ' ' -f 1-3`
#echo $load

eval `head -n 1 /proc/stat | awk -v idle1=0 -v sum1=0 '{for (i=2;i<=8;i++){sum1=sum1+$i} printf("sum1=%.0f; idle1=%.0f;",sum1,$5)}'`
sleep 0.5
eval `head -n 1 /proc/stat | awk -v idle2=0 -v sum2=0 '{for (i=2;i<=8;i++){sum2=sum2+$i} printf("sum2=%.0f; idle2=%.0f;",sum2,$5)}'`

cpu_use=`echo "scale=4;(1-($idle2-$idle1)/($sum2-$sum1))*100" | bc`
cpu_use=`printf "%.2f" $cpu_use`

wendu=`cat /sys/class/thermal/thermal_zone0/temp | awk '{printf("%.2f", $1/1000)}'`
#echo $wendu
#warning=`echo $wendu | awk '{if($1>70){printf ("warning")}else if($1>50){printf ("note")} else {printf ("normal")}}'` 

#wendu=`cat /sys/class/thermal/thermal_zone0/temp`
#wendu=`printf "%.2f" $wendu/1000`
#echo $wendu
if [[ `echo $wendu '>=' 70 | bc -l` == 1 ]];then
    warn="warning"
elif [[ `echo $wendu '>=' 50 | bc -l` == 1 ]];then
    warn="note"
else
    warn="normal"
fi
#echo $warning

echo "$dateinfo $load $cpu_use $wendu°C $warn"
