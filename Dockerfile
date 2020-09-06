FROM ubuntu:trusty

# Install build dependencies
RUN apt-get update
RUN apt-get -y install software-properties-common
RUN add-apt-repository -y ppa:beineri/opt-qt597-trusty
RUN add-apt-repository -y ppa:george-edison55/cmake-3.x
RUN apt-get update
RUN apt-get -y install build-essential mesa-common-dev qt59base qt59serialport cmake mercurial subversion git wget libfuse2

# Define environment variable
ENV PATH /opt/qt59/bin/:$PATH

WORKDIR /serialplot
ADD . /serialplot
WORKDIR ./build_docker
RUN cmake ../ && make appimage
