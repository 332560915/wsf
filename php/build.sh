#!/bin/bash

set -e
./autogen.sh
cp build/wsf_c/Makefile.am wsf_c/
cd wsf_c
build/copy_build_files.sh
cd ..
cp build/wsf_c/axis2c/configure.ac wsf_c/axis2c
cp build/wsf_c/axis2c/util/configure.ac wsf_c/axis2c/util
cp build/wsf_c/axis2c/axiom/configure.ac wsf_c/axis2c/axiom
cp build/wsf_c/rampartc/configure.ac wsf_c/rampartc

for i in `find . -name configure.ac`
do
    sed  's/-Werror//' $i > configure.ac.back;
    mv configure.ac.back $i;
done
./autogen.sh && ./configure --with-wsf --enable-openssl --with-apache2=/usr/local/apache2/include --with-axis2=`pwd`/wsf_c/axis2c/include --enable-tests=no --enable-wsclient=no  --enable-savan=no --prefix=`php-config --extension-dir`/wsf_c && make -j 100 

