FROM alpine:latest AS build

ARG SSH_PUBLIC_KEY
ARG SSH_PRIVATE_KEY

ARG REF
ENV REF=${REF}

ARG REMOTE
ENV REMOTE=${REMOTE}

ARG BUILD_DIR=/tmp/build
ENV BUILD_DIR=${BUILD_DIR}

ARG INSTALL_DIR=/tmp/install
ENV INSTALL_DIR=${INSTALL_DIR}

ARG ROOT_DIR=/tmp/git
ENV ROOT_DIR=${ROOT_DIR}

ARG ARCHIVE_OUTPUT_DIR=/usr/local/lib
ENV ARCHIVE_OUTPUT_DIR=${ARCHIVE_OUTPUT_DIR}
ARG LIBRARY_OUTPUT_DIR=/usr/local/lib
ENV LIBRARY_OUTPUT_DIR=${LIBRARY_OUTPUT_DIR}
ARG RUNTIME_OUTPUT_DIR=/usr/local/bin
ENV RUNTIME_OUTPUT_DIR=${RUNTIME_OUTPUT_DIR}

RUN apk add --no-cache \
    g++ \
    make \
    cmake \
    git \
    openssh-client 

RUN mkdir -p /root/.ssh \
    && echo "${SSH_PUBLIC_KEY}" > /root/.ssh/id_rsa.pub \
    && echo "${SSH_PRIVATE_KEY}" > /root/.ssh/id_rsa \
    && chmod 600 /root/.ssh/* \
    && ssh-keyscan -H github.com >> /root/.ssh/known_hosts
    

FROM build as git

WORKDIR ${ROOT_DIR}
RUN \
    #git clone --single-branch --branch ${REF} ${REMOTE} ${GIT_DIR} \
    #&& git pull && git submodule init && git submodule update && git submodule status
    git init .\
    && git remote add origin ${REMOTE} \
    && git fetch origin ${REF} \
    && git checkout FETCH_HEAD \
    && git submodule init \
    && git submodule update \
    && git git submodule status

WORKDIR ${BUILD_DIR}
RUN ls -la ${ROOT_DIR} && cmake ${ROOT_DIR} \
    && cmake --build . --parallel \
    && make install

FROM scratch
COPY --from=build ${INSTALL_DIR} ${INSTALL_DIR}
