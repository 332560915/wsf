#!/bin/bash
#./autogen.sh

set -e 

#if [ -d  "axis2c" ];then
#    cd axis2c;
#    svn up;
#    cd ..
#fi

#    cp build/axis2c/configure.ac axis2c

rm -rf axis2c/src/core/transport/xmpp
rm -rf axis2c/samples/server/listener
cp -r build/xmpp/xmpp axis2c/src/core/transport
cp -r build/xmpp/include/*.h axis2c/include
cp -r build/xmpp/samples/listener axis2c/samples/server
cp build/rampartc/configure.ac rampartc
cp build/src/core/engine/Makefile.am src/core/engine/

./autogen.sh
./configure --prefix=`pwd`/deploy --enable-openssl=yes --enable-rampart=yes --enable-xmpp=no --with-axis2=`pwd`/axis2c/include 
make


