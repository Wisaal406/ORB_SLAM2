# Docker Support

#### Pre-requisite

- [What are Containers?](https://aws.amazon.com/containers)
- [What is Docker?](https://en.wikipedia.org/wiki/Docker_(software))

## Description

Docker support is added to ORB-SLAM2 using which user can perform following operations

- Build ORB-SLAM2 from docker
- Run ORB-SLAM2 applications from docker
- Create docker image

### Build ORB-SLAM2 from docker
We can build ORB-SLAM2 using docker, advantages of this method is user need not have to install any dependences in host computer. Docker base image with installed dependences is used to compile ORB-SLAM2

```
cd ORB-SLAM2
./scripts/docker/build-from-docker.sh
```

Output build artifacts are stored under `ORB-SLAM2/products`

### Run ORB-SLAM2 applications from docker
Similar to building ORB-SLAM2 using a docker image, we can also run the built ORB-SLAM2 binaries in the same docker image

```
cd ORB-SLAM2
./scripts/docker/run-from-docker.sh "<application-binary> <parameter>"
```

### Create Docker image
Optionally user can create new docker image locally which can be used to build or run ORB-SLAM2.

```
cd ORB-SLAM2
./scripts/docker/create-image.sh
```
Above script creates a docker image, provisioned with required dependences for ORB-SLAM2

#### Reference
- [Using GUI's with Docker](http://wiki.ros.org/docker/Tutorials/GUI)
- [Using Hardware Acceleration with Docker](http://wiki.ros.org/docker/Tutorials/Hardware%20Acceleration)
