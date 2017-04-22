#!/bin/sh

threadSize=`nproc`
random=`od -vAn -N4 -tu4 < /dev/random`
loginName="lkujira_$(($random % 1000))"
host="godwhale.ddns.net"
port=4082

echo -n "please input thread size (default is $threadSize): "
read threadSize2

if [ ! x"$threadSize2" = x"" ]; then
    threadSize="$threadSize2"
fi

while :
do
    ./godwhale_child "$host" "$port" "$loginName" "$threadSize"
    sleep 10
done
