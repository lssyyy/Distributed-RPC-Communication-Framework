#######################################################
# File Name: autobuild.sh
# Author: li shi yuan
# Email: lishiyuan01@163.com
# Created Time: 2022年06月10日 星期五 18时40分28秒
#!/bin/bash

set -e

rm -rf `pwd`/build/*
cd `pwd`/build &&
    cmake .. &&
    make
cd ..
cp -r `pwd`/src/include `pwd`/lib
