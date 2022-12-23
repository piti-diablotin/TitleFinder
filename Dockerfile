FROM ubuntu:latest

USER root

ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install nlohmann-json3-dev libspdlog-dev libcurl4-openssl-dev libfmt-dev  cmake g++ git -y


WORKDIR /tmp

RUN git clone https://github.com/piti-diablotin/TitleFinder.git

WORKDIR ./TitleFinder
RUN mkdir build

WORKDIR ./build
RUN cmake .. -DCMAKE_BUILD_TYPE=Release
RUN make -j $(nproc --all)

RUN rm -rf /var/lib/apt/lists/*

ENTRYPOINT [ "/tmp/TitleFinder/build/cli/titlefinder_cli" ]
