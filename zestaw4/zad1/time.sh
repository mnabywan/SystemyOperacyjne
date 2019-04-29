#!/usr/bin/env bash
while :
do
date | awk '{print $4}'
sleep 1
done
