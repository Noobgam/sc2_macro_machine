FROM ubuntu:20.04

RUN apt-get update -qq
RUN DEBIAN_FRONTEND=noninteractive apt-get install -yq --no-install-recommends cmake clang make git ca-certificates zip
RUN update-ca-certificates -f
