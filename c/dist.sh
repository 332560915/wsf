#!/bin/sh
cd sandesha2c/samples
find . -name "*.c" | xargs sed -i "s/8888/9090/g"
find . -name "*.c" | xargs sed -i "s/7777/6060/g"
cd ../../
cd wso2-wsf-c-src-1.0
mv axis2c/xdocs docs/axis2c
mv savanc/xdocs docs/savanc
mv sandesha2c/xdocs docs/sandesha2c
mv rampartc/xdocs docs/rampartc
find -name "*.libs"| xargs rm -rf
find -name "*.deps"| xargs rm -rf
find -name "*.svn"| xargs rm -rf
find -name "*.Makefile"| xargs rm -rf
cd ..
