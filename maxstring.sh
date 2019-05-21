#!/bin/bash

Max=0
MaxString=0

function Fliter() {
    FliterFile=$1
        file ${FliterFile} | grep -q -w "text"
    if [[ $? -eq 0 ]]; then
        echo 0
    else
        echo 1
    fi
}

function FindFile() {
    local File=$1
    for i in `cat ${File} | tr -c -s "A-Za-z" "\n"`; do 
        if [[ ${Max} -lt ${#i} ]]; then
            Max=${#i}
            MaxString=${i}
        fi
    done
}

function FindDir() {
    local DirName=$1
    for i in `ls -A ${DirName}`; do
        if [[ -d ${DirName}/${i} ]]; then
            FindDir ${DirName}/${i}
        elif [[ -f ${DirName}/${i} ]];then
            Check=`Fliter ${DirName}/${i}`
            if [[ ${Check} -eq 0 ]]; then
                FindFile ${DirName}/${i}
            fi
        fi
    done
}

cnt=1
for k in "$@"; do
    if [[ -d $k ]];then
        FindDir $k
    elif [[ -f $k ]];then
        Check=`Fliter $k`
        if [[ ${Check} -eq 0 ]]; then
            FindFile $k
        fi
    else 
        cnt=0
        printf "you input error\n"
        break
    fi
done
if [[ cnt -eq 1 ]]; then
    printf "MaxString = %s\nMax = %d\n" ${MaxString} ${Max}
fi
