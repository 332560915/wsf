cd wsf_c
grep -rl AXIS2C_HOME *| xargs sed -i "s/AXIS2C_HOME/WSFC_HOME/g"
export WSFC_HOME=`pwd`/deploy
./configure --prefix=$WSFC_HOME;
make
make install
make samples
cd ..
cp wsf_c_dist_hook.sh wsf_c/dist_hook.sh
sed -i "s/src//g" Makefile.am
./configure --with-wsf --with-apache2=/usr/local/apache2/include --with-axis2=`pwd`/wsf_c/axis2c/include --enable-tests=yes --enable-savan=no --enable-xmpp=yes --prefix=`php-config --extension-dir`/wsf_c
make dist
tar xzf wso2-wsf-php-1.0.0beta.tar.gz
rm wso2-wsf-php-1.0.0beta.tar.gz
cp -r src samples scripts docs wso2-wsf-php-1.0.0beta

cd wso2-wsf-php-1.0.0beta

rm -rf `find . -type d -name .svn`
rm -rf `find . -type f -name *.la`
rm -rf `find . -type f -name *.o`
rm -rf `find . -type f -name *.lo`
rm -rf `find . -type f -name *.loT`
rm -rf `find . -type d -name .deps`
rm -rf `find . -type d -name .libs`
rm -rf `find . -type f -name Makefile`
rm -rf `find . -type d -name autom4te.cache`

sed -i "s/wsf_c/wsf_c src/g" Makefile.in
sed -i "s/ac_default_prefix=\/usr\/local\/wsf_c/ac_default_prefix=\`php-config \-\-extension\-dir\`\/wsf_c/g" configure
sed -i "s/subdirs wsf_c/subdirs wsf_c src/g" configure
sed -i "s/savanc//g" wsf_c/configure
sed -i "s/\tcp -rf samples\/*.*//g" wsf_c/sandesha2c/Makefile.in
sed -i "s/\tcp -rf samples\/*.*//g" wsf_c/rampartc/Makefile.in

cd ..
tar czf wso2-wsf-php-1.0.0beta.tar.gz wso2-wsf-php-1.0.0beta
zip -rq wso2-wsf-php-1 wso2-wsf-php-1.0.0beta
mv wso2-wsf-php-1.zip wso2-wsf-php-1.0.0beta.zip
rm -rf wso2-wsf-php-1.0.0beta
