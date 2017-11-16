#!/bin/bash

CurDir=$(realpath $(dirname $0))
ProjectDir=$(realpath ${CurDir}/../..)
cd ${ProjectDir}

if [ "$1" = "" ]
then
    echo "Provide video path"
    exit 1
fi

xhost +local:root
docker run -it \
    --net=host \
    --ipc=host \
    --device=/dev/dri:/dev/dri \
    --env="DISPLAY" \
    --env="QT_X11_NO_MITSHM=1" \
    --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
    --volume="$(pwd):/root/orb-slam2" \
    shanmukhananda/orb-slam2:latest \
    /bin/bash -c \
    "/root/orb-slam2/run_slam_on_garching_test_drive.sh \
    ${1}"
