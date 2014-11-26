#!/bin/bash

# control kitchen window light on GPIO #10
python3 /home/eduolin/git/pysmarthouse/GpioOnSunSchedule/GpioOnSunSchedule.py -c "Stockholm" -p 10 -s 1 -d 1 -i 300 &