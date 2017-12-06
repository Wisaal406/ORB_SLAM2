#!/bin/bash

ScriptDir=$(realpath $(dirname $0))
ProjectDir=$(realpath ${ScriptDir}/..)
cd ${ProjectDir}

if [ "$1" = "" ]
then
    echo "Provide command to run"
    exit 1
fi

xhost +local:root
docker run --rm -it \
    --net=host \
    --ipc=host \
    --device=/dev/dri:/dev/dri \
    --env="DISPLAY" \
    --env="QT_X11_NO_MITSHM=1" \
    --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
    --volume="$(pwd):/root/orb-slam2" \
    shanmukhananda/orb-slam2:latest \
    /bin/bash -c \
    "${1}"
