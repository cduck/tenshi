#!/bin/bash -xe

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

cd tools

# Extract the cross-gcc
if [ ! -e arm-toolchain ]
then
	if [ ! -e project-tenshi-toolchain.tar.bz2 ]
	then
		wget https://rqou.com/project-tenshi-tools/project-tenshi-toolchain.tar.bz2 --no-check-certificate
	fi
	tar xjf project-tenshi-toolchain.tar.bz2

	if [ ! -e project-tenshi-openocd.tar.bz2 ]
	then
		wget https://rqou.com/project-tenshi-tools/project-tenshi-openocd.tar.bz2 --no-check-certificate
	fi
	tar xjf project-tenshi-openocd.tar.bz2
fi

which emcc >/dev/null || {
	# Extract emscripten
	if [ ! -e emscripten-bin ]
	then
		if [ ! -e emscripten-bin-18apr2014.tar.bz2 ]
		then
			wget http://rqou.com/emscripten-bin-18apr2014.tar.bz2 --no-check-certificate
		fi
		tar xjf emscripten-bin-18apr2014.tar.bz2
	fi
}

cd ..
