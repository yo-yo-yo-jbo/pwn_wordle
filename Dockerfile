FROM gcc:14.2-bookworm as build

WORKDIR /usr/src/app
COPY . .

RUN set -eux; \
    \
    chmod +x ./compile.sh; \
    ./compile.sh


FROM python:3.12-slim-bookworm as core

ARG USERNAME=app
ARG PASSWORD=app
ARG GOTTY_VERSION=1.0.1

COPY generate.sh /generate.sh
COPY dictionary.txt wordle flag /opt/wordle/
COPY --from=build /usr/src/app/wordle.elf /opt/wordle/

RUN set -eux; \
    apt-get update; \
    apt-get install -y sudo xxd cron curl; \
    \
    chmod -r /home; \
    chmod +x /generate.sh; \
    echo "%sudo ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/sudo; \
    adduser --disabled-password --no-create-home --ingroup sudo --shell /generate.sh --gecos ${USERNAME} ${USERNAME}; \
    echo "${USERNAME}:${PASSWORD}" | chpasswd; \
    # mount -o remount,rw,hidepid=2 /proc; \
    \
    curl -L https://github.com/yudai/gotty/releases/download/v${GOTTY_VERSION}/gotty_linux_$(dpkg --print-architecture).tar.gz \
      -o gotty.tar.gz; \
    tar -xzf gotty.tar.gz -C /usr/local/bin/; \
    rm -f gotty.tar.gz; \
    chmod +x /usr/local/bin/gotty

RUN set -eux; \
    apt-get update; \
    apt-get install -y --no-install-recommends vim nano; \
    \
    pip install --no-cache-dir --upgrade pwntools ipython cron-descriptor; \
    \
    adduser --disabled-password --no-create-home --shell /sbin/nologin --gecos flag flag; \
    \
    chmod 400 /opt/wordle/flag; \
    chmod 4555 /opt/wordle; \
    chown flag:root /opt/wordle/flag; \
    chmod +x /opt/wordle/wordle*

EXPOSE 3000
USER $USERNAME
ENV TERM=xterm-256color
CMD [ "gotty", "--address", "0.0.0.0", "--port", "3000", "--permit-write", "--reconnect", "/generate.sh" ]
