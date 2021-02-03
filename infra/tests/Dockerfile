FROM ubuntu:20.04

RUN apt-get update -qq
RUN DEBIAN_FRONTEND=noninteractive apt-get install -yq libasan5 wget vim ca-certificates lsb-release wget software-properties-common

RUN update-ca-certificates -f

RUN bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

RUN useradd -ms /bin/bash dummy
WORKDIR /home/dummy
USER dummy
ENV PATH $PATH

RUN wget 'https://noobgam-public.s3.eu-central-1.amazonaws.com/SC2.4.10.zip'

# i am too lazy to reinstall it :(
USER root
RUN DEBIAN_FRONTEND=noninteractive apt-get install -yq unzip
USER dummy

RUN unzip -P iagreetotheeula SC2.4.10.zip
RUN rm SC2.4.10.zip

RUN mkdir -p /home/dummy/StarCraftII/maps/
WORKDIR /home/dummy/StarCraftII/maps/

RUN wget 'https://noobgam-public.s3.eu-central-1.amazonaws.com/2020S2LadderMaps.zip'
RUN unzip 2020S2LadderMaps.zip
RUN rm 2020S2LadderMaps.zip

WORKDIR /home/dummy

USER root
RUN ln -s /usr/lib/llvm-11/bin/llvm-symbolizer  /usr/bin/llvm-symbolizer
ENV ASAN_SYMBOLIZER_PATH=/usr/bin/llvm-symbolizer
USER dummy

ENTRYPOINT [ "/bin/bash" ]