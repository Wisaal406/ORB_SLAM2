# docker image to run ORB-SLAM2
FROM ubuntu:18.04
MAINTAINER devin@monodrive.io

# setup
ENV BASE_DIR /home/vo
ENV DEBIAN_FRONTEND noninteractive
WORKDIR ${BASE_DIR}
RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y \
    git \
    wget

# c++
RUN apt-get install -y \
    build-essential \
    gcc \
    cmake

# pangolin
RUN apt-get install -y \
    libgl1-mesa-dev \
    libglew-dev

RUN git clone https://github.com/stevenlovegrove/Pangolin.git ${BASE_DIR}/pangolin
WORKDIR ${BASE_DIR}/pangolin/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN cmake --build . -- -j8

# opencv
RUN apt-get install -y \
    pkg-config \
    libgtk2.0-dev \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev

RUN git clone https://github.com/opencv/opencv.git --branch 3.2.0 --depth 1 ${BASE_DIR}/opencv
WORKDIR ${BASE_DIR}/opencv/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j8
RUN make install

# eigen
WORKDIR ${BASE_DIR}/dump
RUN wget http://bitbucket.org/eigen/eigen/get/3.3.7.tar.gz
RUN tar -xzf 3.3.7.tar.gz
RUN mv eigen-eigen-323c052e1731 /usr/local/include/eigen

# orb-slam2
RUN git clone https://github.com/monoDriveIO/ORB_SLAM2 --branch dockerize --depth 1 ${BASE_DIR}/orbslam2

# orb-slam2 // dbow2
WORKDIR ${BASE_DIR}/orbslam2/Thirdparty/DBoW2/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j8

# orb-slam2 // g2o
WORKDIR ${BASE_DIR}/orbslam2/Thirdparty/g2o/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j8

# orb-slam2 // vocab
WORKDIR ${BASE_DIR}/orbslam2/Vocabulary
RUN tar -xf ORBvoc.txt.tar.gz

# orb-slam2 // build
WORKDIR ${BASE_DIR}/orbslam2/build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j8

# add some sample files
ADD ./parsed_kitti_dev ${BASE_DIR}/data
ADD ./orbslam_config ${BASE_DIR}/config

# runtime
WORKDIR ${BASE_DIR}
CMD ./orbslam2/Examples/Monocular/mono_kitti orbslam2/Vocabulary/ORBvoc.txt config/monoDrive.yaml data/dataset/sequences/00