#!/usr/bin/env python

# Licensed to Pioneers in Engineering under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  Pioneers in Engineering licenses
# this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License

from __future__ import print_function

import serial
import time
import atexit
import sys



speedArr = []
for i in range(1, len(sys.argv)):
    try:
        speedArr.append(float(sys.argv[i]))
    except ValueError:
        pass
if len(speedArr) <= 0:
    print('Enter a list of speeds as arguments.')
    print('Example:\n\t%s 50.1 -99.3' % sys.argv[0])
    speedArr = [1.0]
print('Speeds: ',speedArr)



filename = '/dev/ttyUSB0'

dev = serial.Serial(filename, 115200, timeout=0)

@atexit.register
def stop_motor():
    print('Ending test.')
    dev.write(b'[ 0x1e 1 3 0 0 0 0 0 0 0 ]\n')
    time.sleep(0.5)

dev.write(b'm\n')
# Switch mode

dev.write(b'4\n')
# I2C

dev.write(b'3\n')
# 100KHz

time.sleep(0.5)

dev.write(b'P\n')
# Enable pull-ups

time.sleep(0.5)

for s in speedArr:
    fixed = int(s*0x10000)
    b1 = fixed>>24 & 0xFF  # High byte of integer
    b2 = fixed>>16 & 0xFF  # Low byte of integer
    b3 = fixed>>8  & 0xFF  # High byte of fraction
    b4 = fixed     & 0xFF  # Low byte of fraction

    dev.write(b'[ 0x1e 1 3 0 0 0 %u %u %u %u ]\n' % (b1, b2, b3, b4))
    # Move motor forward at given speed

    print('Moving at speed: %f (0x%02X%02X.%02X%02X)' % (s, b1, b2, b3, b4))

    time.sleep(1)

dev.write(b'[ 0x1e 1 3 0 0 0 0 0 0 0 ]\n')
# Stop motor
