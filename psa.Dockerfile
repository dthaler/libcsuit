# Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
# SPDX-License-Identifier: BSD-2-Clause
FROM debian:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get -y upgrade
RUN apt-get -y install curl git gcc gdb make
RUN apt-get -y install python3

WORKDIR /root
RUN git clone https://github.com/Mbed-TLS/mbedtls.git
WORKDIR ./mbedtls
RUN git checkout v3.1.0
RUN make install

WORKDIR root
RUN git clone https://github.com/laurencelundblade/QCBOR.git
WORKDIR ./QCBOR
RUN git checkout 11ea361d803589dcfa38767594236afbc8789f8b
RUN make install

WORKDIR /root
RUN git clone https://github.com/laurencelundblade/t_cose.git
WORKDIR ./t_cose
RUN git checkout d5ff4e282d8af34e5756627cf877ab399e7e51af
RUN make -f Makefile.psa libt_cose.a install

WORKDIR /root
RUN ldconfig
COPY . ./libcsuit
RUN mv ./libcsuit/.gdbinit /root
WORKDIR ./libcsuit
RUN make -f Makefile.encode MBEDTLS=1 CC=gcc
RUN make -f Makefile.parser MBEDTLS=1 CC=gcc
RUN make -f Makefile.process MBEDTLS=1 CC=gcc