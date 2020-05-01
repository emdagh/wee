#!/bin/bash

set -ex

SSH_PUBLIC_KEY=$1
SSH_PRIVATE_KEY=$2
TAG=$(git log -1 --pretty=%H)
REF=$(git symbolic-ref HEAD)
REMOTE=$(git remote get-url origin)
CONTAINER=${REF}_tmp
ARTIFACT_DIR=/tmp
INSTALL_DIR=/tmp

docker build \
    --no-cache \
    --target git \
    --build-arg "REF=${REF}" \
    --build-arg "REMOTE=${REMOTE}" \
    --build-arg "SSH_PUBLIC_KEY=$(cat ${SSH_PUBLIC_KEY})" \
    --build-arg "SSH_PRIVATE_KEY=$(cat ${SSH_PRIVATE_KEY})" \
    --tag build:${TAG} \
    .

##
# post-build steps
##
docker tag build:{REF} build:latest
#docker create build:${REF} --name ${CONTAINER}
#docker cp ${CONTAINER}:${INSTALL_DIR} ${ARTIFACT_DIR} #$(Build.BinariesDirectory)
#docker container rm ${CONTAINER}
