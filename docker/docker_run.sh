#!/usr/bin/bash

xhost +

docker run \
	-it \
	--rm \
	--gpus all \
	--net=host \
	--shm-size=8g \
	-e DISPLAY=$DISPLAY \
	-v /tmp/argus_socket:/tmp/argus_socket \
	-v /tmp/.X11-unix/:/tmp/.X11-unix \
	-v $PWD:/workspace/ \
	-w /workspace \
    --name redis-image-container \
	redis-image
