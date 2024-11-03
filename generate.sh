#!/usr/bin/env bash

USERNAME="user-$(head -c8 /dev/urandom | xxd -p | tr -d $'\n')"
DELETE_CRON="${DELETE_CRON:-*/30 * * * *}"

sudo -i bash -c "adduser --gecos ctf --disabled-password --home /home/${USERNAME} --shell /bin/bash ${USERNAME} > /dev/null 2>&1"
sudo -i bash -c "ln -s /opt/wordle /home/${USERNAME}/wordle > /dev/null 2>&1"
sudo -i bash -c "echo '${DELETE_CRON} root pkill -KILL -u ${USERNAME}; userdel -fr ${USERNAME}; rm -f /etc/cron.d/${USERNAME}' > /etc/cron.d/${USERNAME}"
sudo -i bash -c "chmod 751 -R /home/${USERNAME} /var/log/; chmod 751 /etc/passwd /etc/group /home/ /etc/cron.d/ /tmp/"

CRON_TIME_TRANS=$(python -c 'import os, cron_descriptor; print(cron_descriptor.get_description(os.getenv("DELETE_CRON")))')
echo "This user terminate itself ${CRON_TIME_TRANS}"

sudo -i su -l "${USERNAME}"
