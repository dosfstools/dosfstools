# Build Stage:

FROM --platform=linux/amd64 ubuntu:20.04 as builder

## Install build dependencies.
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y gcc autoconf automake gettext make

## Add Source Code
ADD ./dosfstools /dosfstools

WORKDIR /dosfstools

## Build Step
RUN ./autogen.sh
RUN ./configure
RUN make -j4

# Package Stage
FROM --platform=linux/amd64 ubuntu:20.04

COPY --from=builder /dosfstools/src/fatlabel /fatlabel

