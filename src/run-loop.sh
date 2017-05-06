#!/bin/sh

random=`od -vAn -N4 -tu4 < /dev/random`
loginName="lkujira_$(($random % 1000))"
host="godwhale.ddns.net"
port=4082

if [ "$(uname)" = "Darwin" ]; then
    # under Mac OS X platform
    threadSize=`sysctl -n hw.ncpu`
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
    # under GNU/Linux platform
    threadSize=`nproc`
elif [ "$(expr substr $(uname -s) 1 10)" = "MINGW32_NT" ]; then
    # under 32 bits Windows NT platform
    threadSize=$NUMBER_OF_PROCESSORS
elif [ "$(expr substr $(uname -s) 1 10)" = "MINGW64_NT" ]; then
    # under 64 bits Windows NT platform
    threadSize=$NUMBER_OF_PROCESSORS
else
    threadSize=1
fi

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
