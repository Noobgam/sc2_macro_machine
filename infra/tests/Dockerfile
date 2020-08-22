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


