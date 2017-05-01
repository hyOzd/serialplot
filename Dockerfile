FROM ubuntu:trusty

WORKDIR /serialplot
ADD . /serialplot

# Install build dependencies
RUN apt-get update
RUN apt-get -y install software-properties-common
RUN add-apt-repository -y ppa:beineri/opt-qt562-trusty
RUN add-apt-repository -y ppa:george-edison55/cmake-3.x
RUN apt-get update
RUN apt-get -y install build-essential mesa-common-dev qt56base qt56serialport cmake mercurial subversion git wget libfuse2

# Define environment variable
ENV PATH /opt/qt56/bin/:$PATH
