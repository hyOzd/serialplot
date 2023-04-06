FROM ubuntu:bionic AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y software-properties-common && add-apt-repository -y ppa:beineri/opt-qt-5.15.2-bionic && apt-get install -y build-essential mesa-common-dev qt515base qt515serialport qt515svg cmake mercurial subversion git wget libfuse2 libgl1-mesa-dev

# Define environment variable
ENV PATH /opt/qt515/bin/:$PATH

WORKDIR /serialplot
ADD . /serialplot
WORKDIR ./build_docker
RUN cmake ../ && make -j appimage

FROM scratch as exporter
COPY --from=builder /serialplot/build_docker/SerialPlot*.AppImage /
