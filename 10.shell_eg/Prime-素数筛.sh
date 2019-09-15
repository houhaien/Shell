#!/bin/bash

MAX=1000
num=0
sum=0
declare -a prime
declare -a isprime
function init() {
    for i in `seq 1 1000`;do
        eval $1[$i]=0  
    done
}
init prime
init isprime

for((i = 2; i < ${MAX}; i++));
do
    
    if [[ ${prime[$i]} -eq 0 ]];then
        prime[$num]=$i
        let num++
    fi
    for((j = 0; j < ${num}; j++));
    do
        pj=${prime[$j]}
        if [[ `expr ${i}\*$pj` -gt ${MAX} ]];then
            break;
        fi
        prime[`expr ${i}\*${pj}`]=1
        if [[ `expr ${i}%${pj}` == 0 ]];then
            break;
        fi
    done
done
cnt=1
for((i=0;i<$num;i++));
do
    let sum+=${prime[$i]}
    pj=${prime[$i]}
    isprime[$cnt]=$pj
    let cnt++
done
isprime[0]=$sum
#echo "1000以内所有素数的和是：$sum"
#echo "${prime[$1]}"
#echo "你要查询第$1个素数是："
#echo "${isprime[$1]}"
for((i=0;i<$num;i++));do
    echo "${prime[$i]}"
done
echo "$sum"

