#!/bin/bash

if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <duration_in_seconds> [ping_options]"
  exit 1
fi

./ft_ping "${@:2}" > ft_ping.txt &
sleep $1
pkill --signal SIGINT ft_ping

ping "${@:2}" > ping.txt &
sleep $1
pkill --signal SIGINT ping

diff -s -u ping.txt ft_ping.txt