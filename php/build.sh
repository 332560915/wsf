#!/bin/bash

set -e

svn up;
#cp build/axis2c/Makefile.am axis2c


cp build/wsf_c/sandesha2c/Makefile.am wsf_c/sandesha2c
cp build/wsf_c/sandesha2c/src/core/Makefile.am wsf_c/sandesha2c/src/core
cp build/wsf_c/sandesha2c/src/util/Makefile.am wsf_c/sandesha2c/src/util
cp build/wsf_c/sandesha2c/src/wsrm/Makefile.am wsf_c/sandesha2c/src/wsrm
cp build/wsf_c/sandesha2c/src/transport/Makefile.am wsf_c/sandesha2c/src/transport
cp build/wsf_c/sandesha2c/src/handlers/Makefile.am wsf_c/sandesha2c/src/handlers
cp build/wsf_c/sandesha2c/src/storage/Makefile.am wsf_c/sandesha2c/src/storage
cp build/wsf_c/sandesha2c/src/storage/permanent/Makefile.am wsf_c/sandesha2c/src/storage/permanent
cp build/wsf_c/sandesha2c/src/storage/beans/Makefile.am wsf_c/sandesha2c/src/storage/beans
cp build/wsf_c/sandesha2c/src/storage/inmemory/Makefile.am wsf_c/sandesha2c/src/storage/inmemory
cp build/wsf_c/sandesha2c/src/client/Makefile.am wsf_c/sandesha2c/src/client
cp build/wsf_c/sandesha2c/src/msgprocessors/Makefile.am wsf_c/sandesha2c/src/msgprocessors
cp build/wsf_c/sandesha2c/src/polling/Makefile.am wsf_c/sandesha2c/src/polling
cp build/wsf_c/sandesha2c/src/workers/Makefile.am wsf_c/sandesha2c/src/workers
cp build/wsf_c/sandesha2c/autogen.sh wsf_c/sandesha2c


cp build/wsf_c/axis2c/configure.ac wsf_c/axis2c
cp build/wsf_c/axis2c/rampart/samples/Makefile.am wsf_c/axis2c/rampart/samples
cp build/wsf_c/axis2c/rampart/samples/callback/Makefile.am wsf_c/axis2c/rampart/samples/callback
cp build/wsf_c/axis2c/rampart/samples/callback/htpasswd_callback/Makefile.am wsf_c/axis2c/rampart/samples/callback/htpasswd_callback
cp build/wsf_c/axis2c/rampart/configure.ac wsf_c/axis2c/rampart
cp build/wsf_c/axis2c/rampart/Makefile.am wsf_c/axis2c/rampart
cp build/wsf_c/axis2c/rampart/autogen.sh wsf_c/axis2c/rampart
cp build/wsf_c/sandesha2c/src/core/Makefile.am wsf_c/sandesha2c/src/core
cp build/wsf_c/sandesha2c/Makefile.am wsf_c/sandesha2c/
cp build/wsf_c/axis2c/Makefile.am wsf_c/axis2c/
cp wsf_c/axis2c/samples/server/axis2.xml .

./autogen.sh && ./configure --with-wsf --prefix=`php-config --extension-dir`/wsf_c && make

