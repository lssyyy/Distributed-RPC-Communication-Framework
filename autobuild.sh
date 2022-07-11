#######################################################
# File Name: autobuild.sh
# Author: li shiyuan
# Email: lishiyuan01@163.com
# Created Time: 2022年05月21日 星期六 18时40分28秒
#!/bin/bash

set -e

rm -rf `pwd`/build/*
cd `pwd`/build &&
    cmake .. &&
    make
cd ..
cp -r `pwd`/src/include `pwd`/lib
