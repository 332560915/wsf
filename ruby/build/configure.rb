#!/usr/bin/env ruby

require 'mkmf'

# Generate WSFC Wrapper
system ('swig -ruby -I../swig/ -outdir ./ -Wall WSFC.i') unless File.exists? ('WSFC_wrap.c')

# Create "lib" Directory
Dir.mkdir ('lib') unless (FileTest.exist? ('lib') && FileTest.directory? ('lib'))

# Create "lib" Directory
Dir.mkdir ('lib/wsf') unless (FileTest.exist? ('lib/wsf') && FileTest.directory? ('lib/wsf'))

system ('cp -r ../src/*.rb lib')
system ('cp -r ../src/wsf/*.rb lib/wsf')

# Add Build Rules
dir_config ('WSFC', '/home/janapriya/deploy/axis2c/include/axis2-1.1', '/home/janapriya/deploy/axis2c/lib')

have_library ('axutil')
have_library ('axis2_minizip')
have_library ('axis2_parser')
have_library ('axis2_libxml2')
have_library ('axis2_axiom')
have_library ('axis2_engine')
have_library ('axis2_http_common')
have_library ('axis2_http_sender')
have_library ('axis2_http_receiver')
have_library ('axis2_engine')

create_makefile ('WSFC')
