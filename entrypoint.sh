#!/usr/bin/env bash

set -e

# Start the cron daemon in the background
echo "Starting cron daemon..."
sudo service cron start

# Start gotty as the $USERNAME user
echo "Starting gotty"
gotty --address 0.0.0.0 --port 3000 --permit-write --reconnect /generate.sh