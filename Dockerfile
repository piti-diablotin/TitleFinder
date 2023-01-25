ARG RELEASE=latest
FROM ubuntu:$RELEASE as build

USER root

ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install nlohmann-json3-dev libspdlog-dev libcurl4-openssl-dev libfmt-dev libavformat-dev libavutil-dev libavcodec-dev cmake g++ git -y


WORKDIR /tmp

RUN git clone https://github.com/piti-diablotin/TitleFinder.git

WORKDIR ./TitleFinder
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DUSE_HEADER_ONLY=ON && cmake --build build -j $(nproc --all)

FROM ubuntu:$RELEASE as main

ENV DEBIAN_FRONTEND=noninteractive
RUN apt update && apt install libcurl4 libavformat58 libavutil56 libavcodec58 -y && apt clean
COPY --from=build /tmp/TitleFinder/build/cli/titlefinder_cli /usr/bin/

ENTRYPOINT [ "/usr/bin/titlefinder_cli" ]
