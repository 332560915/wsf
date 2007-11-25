#!/bin/bash
set -e
grep -rl WSFC_HOME *| xargs sed -i "s/WSFC_HOME/WSFCPP_HOME/g"
grep -rl AXIS2C_HOME *| xargs sed -i "s/AXIS2C_HOME/WSFCPP_HOME/g"

cp build/wsf_c/rampartc/configure.ac wsf_c/rampartc/configure.ac
cp build/wsf_c/rampartc/src/Makefile.am  wsf_c/rampartc/src/Makefile.am
cp build/wsf_c/rampartc/src/core/Makefile.am  wsf_c/rampartc/src/core/Makefile.am
cp build/wsf_c/axis2c/neethi/configure.ac wsf_c/axis2c/neethi/configure.ac
cp build/wsf_c/axis2c/neethi/src/Makefile.am wsf_c/axis2c/neethi/src/Makefile.am


sh autogen.sh

./configure --prefix=`pwd`/deploy 
make -j 10 
make install
make examples
