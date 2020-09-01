FROM ubuntu:20.04

RUN apt-get update -qq
RUN DEBIAN_FRONTEND=noninteractive apt-get install -yq --no-install-recommends libasan4 wget vim ca-certificates zip unzip

RUN update-ca-certificates -f

RUN useradd -ms /bin/bash dummy
WORKDIR /home/dummy
USER dummy
ENV PATH $PATH

RUN wget 'https://noobgam-public.s3.eu-central-1.amazonaws.com/SC2.4.10.zip'
RUN unzip -P iagreetotheeula SC2.4.10.zip
RUN rm SC2.4.10.zip

RUN mkdir -p /home/dummy/StarCraftII/maps/
WORKDIR /home/dummy/StarCraftII/maps/

RUN wget 'https://noobgam-public.s3.eu-central-1.amazonaws.com/2020S2LadderMaps.zip'
RUN unzip 2020S2LadderMaps.zip
RUN rm 2020S2LadderMaps.zip

WORKDIR /home/dummy


ENTRYPOINT [ "/bin/bash" ]
