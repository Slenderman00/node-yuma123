FROM node:23.11-bookworm

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    curl \
    wget \
    vim \
    git \
    python3 \
    python3-pip \
    libyuma-dev \
    libyangrpc-dev \
    netconfd \
    yangcli \
    libxml2-dev \
    openssh-server \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN ln -sf /usr/include/libxml2/libxml /usr/include/libxml

RUN useradd -m -s /bin/bash pi && \
    echo "pi:raspberry" | chpasswd

RUN echo "Port 830" >> /etc/ssh/sshd_config && \
    echo "Subsystem netconf /usr/sbin/netconf-subsystem" >> /etc/ssh/sshd_config && \
    echo "PasswordAuthentication yes" >> /etc/ssh/sshd_config

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

RUN mkdir node-yuma123
COPY . /node-yuma123
RUN cd /node-yuma123 && rm -rf node_modules || true && rm package-lock.json || true

WORKDIR /node-yuma123
RUN npm install

ENTRYPOINT ./entrypoint.sh