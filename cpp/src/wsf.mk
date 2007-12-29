AUTOCONF = .\..\configure.in
!include $(AUTOCONF)

WSFCPP_HOME_DIR="%WSFCPP_HOME%"

CFLAGS = /nologo /w /D "WIN32" /D "_WINDOWS" /D "USE_SANDESHA2" /D "_MBCS" 

LDFLAGS = /nologo /LIBPATH:$(WSFCPP_HOME_DIR)\lib /LIBPATH:$(LIBXML2_BIN_DIR)\lib /IGNORE:4089

LIBS = axutil.lib axis2_engine.lib axis2_parser.lib \
       axiom.lib libxml2.lib wsock32.lib axis2_http_sender.lib \
       neethi_util.lib neethi.lib sandesha2.lib mod_rampart.lib

INCLUDE_PATH = /I$(WSFCPP_HOME_DIR)\include /I$(LIBXML2_BIN_DIR)\include /I$(ICONV_BIN_DIR)\include \
       /I.\..\include

!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS) /D "_DEBUG" /Od /Z7 $(CRUNTIME)d
LDFLAGS = $(LDFLAGS) /DEBUG
!else
CFLAGS = $(CFLAGS) /D "NDEBUG" /O2 $(CRUNTIME)
LDFLAGS = $(LDFLAGS)
!endif

wso2_wsf_dll :
	@if not exist int.msvc mkdir int.msvc
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) *.cpp /Foint.msvc\ /c
             @rc.exe /r /fo "int.msvc\wsf.res" wsf.rc
	@link.exe $(LDFLAGS) int.msvc\*.obj int.msvc\wsf.res $(LIBS) /DLL  /OUT:$(WSFCPP_HOME_DIR)\lib\wso2_wsf.dll /IMPLIB:$(WSFCPP_HOME_DIR)\lib\wso2_wsf.lib

wsfcpp: wso2_wsf_dll

cleanint:
	@if exist $(WSFCPP_HOME_DIR)\lib\wso2_wsf.ilk del $(WSFCPP_HOME_DIR)\lib\wso2_wsf.ilk

clean: 
	@if exist int.msvc rmdir /s /q int.msvc
		
copy_sqlite:
	@if exist $(SQLITE_BIN_DIR)\sqlite3.dll copy /Y  $(SQLITE_BIN_DIR)\sqlite3.dll $(WSFCPP_HOME_DIR)\lib
	@if exist $(SQLITE_BIN_DIR)\sqlite3.exe copy /Y  $(SQLITE_BIN_DIR)\sqlite3.exe $(WSFCPP_HOME_DIR)\lib

dist: clean copy_sqlite wsfcpp cleanint
