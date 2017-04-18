#!/bin/sh

threads=`nproc`
random=`od -vAn -N4 -tu4 < /dev/random`
loginName="lkujira_$(($random % 1000))"
host="godwhale.ddns.net"
port=4082

while :
do
    echo "./godwhale_child $host $port $loginName $threads"
done
