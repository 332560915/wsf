AUTOCONF = .\..\configure.in
!include $(AUTOCONF)

# WSFCPP_HOME_DIR="%WSFCPP_HOME%"

WSFCPP_HOME_DIR=.\..\wso2-wsf-cpp-bin-$(WSFCPP_VERSION)-win32

CLIENT_SAMPLES_HOME_DIR=.\clients
SERVICES_SAMPLES_HOME_DIR=.\services
WSFCPP_SAMPLES_DIR=$(WSFCPP_HOME_DIR)\bin\samples

CFLAGS = /nologo /w /D "WIN32" /D "_WINDOWS" /D "_MBCS" /EHsc

CC=@cl.exe
LD=@link.exe

LDFLAGS = /nologo /LIBPATH:$(WSFCPP_HOME_DIR)\lib

LIBS = axutil.lib axis2_engine.lib axis2_parser.lib axiom.lib wso2_wsf.lib

INCLUDE_PATH = /I$(WSFCPP_HOME_DIR)\include /I.\..\include /I$(WSFCPP_HOME_DIR)\include\platforms

!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS) /D "_DEBUG" /Od /Z7 $(CRUNTIME)d /D "AXIS2_DECLARE_EXPORT"
LDFLAGS = $(LDFLAGS) /DEBUG
!else
CFLAGS = $(CFLAGS) /D "NDEBUG" /O2 $(CRUNTIME) /D "AXIS2_DECLARE_EXPORT"
LDFLAGS = $(LDFLAGS)
!endif

!if "$(EMBED_MANIFEST)" == "0"
_VC_MANIFEST_EMBED_EXE=
_VC_MANIFEST_EMBED_DLL=
!else
_VC_MANIFEST_EMBED_EXE= if exist $@.manifest $(MT) -nologo -manifest $@.manifest -outputresource:$@;1
_VC_MANIFEST_EMBED_DLL= if exist $@.manifest $(MT) -nologo -manifest $@.manifest -outputresource:$@;2
!endif

int_dir:
	@if not exist int.msvc mkdir int.msvc

echo_dir:
	@if not exist int.msvc\echo mkdir int.msvc\echo
	@if not exist $(WSFCPP_SAMPLES_DIR)\echo mkdir $(WSFCPP_SAMPLES_DIR)\echo
             
echo_exe:
	@if not exist int.msvc\echo\echo mkdir int.msvc\echo\echo
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\echo\echo.cpp /Foint.msvc\echo\echo\ /c
	$(LD) $(LDFLAGS) int.msvc\echo\echo\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\echo\echo.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

echo_blocking_exe:
	@if not exist int.msvc\echo\echo_blocking mkdir int.msvc\echo\echo_blocking
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\echo\echo_blocking.cpp /Foint.msvc\echo\echo_blocking\ /c
	$(LD) $(LDFLAGS) int.msvc\echo\echo_blocking\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\echo\echo_blocking.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

echo_blocking_addr_exe:
	@if not exist int.msvc\echo\echo_blocking_addr mkdir int.msvc\echo\echo_blocking_addr
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\echo\echo_blocking_addr.cpp /Foint.msvc\echo\echo_blocking_addr\ /c
	$(LD) $(LDFLAGS) int.msvc\echo\echo_blocking_addr\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\echo\echo_blocking_addr.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

echo_blocking_dual_exe:
	@if not exist int.msvc\echo\echo_blocking_dual mkdir int.msvc\echo\echo_blocking_dual
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\echo\echo_blocking_dual.cpp /Foint.msvc\echo\echo_blocking_dual\ /c
	$(LD) $(LDFLAGS) int.msvc\echo\echo_blocking_dual\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\echo\echo_blocking_dual.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

echo_blocking_soap11_exe:
	@if not exist int.msvc\echo\echo_blocking_soap11 mkdir int.msvc\echo\echo_blocking_soap11
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\echo\echo_blocking_soap11.cpp /Foint.msvc\echo\echo_blocking_soap11\ /c
	$(LD) $(LDFLAGS) int.msvc\echo\echo_blocking_soap11\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\echo\echo_blocking_soap11.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

echo_non_blocking_exe:
	@if not exist int.msvc\echo\echo_non_blocking mkdir int.msvc\echo\echo_non_blocking
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\echo\echo_non_blocking.cpp /Foint.msvc\echo\echo_non_blocking\ /c
	$(LD) $(LDFLAGS) int.msvc\echo\echo_non_blocking\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\echo\echo_non_blocking.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

echo_non_blocking_dual_exe:
	@if not exist int.msvc\echo\echo_non_blocking_dual mkdir int.msvc\echo\echo_non_blocking_dual
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\echo\echo_non_blocking_dual.cpp /Foint.msvc\echo\echo_non_blocking_dual\ /c
	$(LD) $(LDFLAGS) int.msvc\echo\echo_non_blocking_dual\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\echo\echo_non_blocking_dual.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

echo_rest_exe:
	@if not exist int.msvc\echo\echo_rest mkdir int.msvc\echo\echo_rest
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\echo\echo_rest.cpp /Foint.msvc\echo\echo_rest\ /c
	$(LD) $(LDFLAGS) int.msvc\echo\echo_rest\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\echo\echo_rest.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

echo_security_exe:
	@if not exist int.msvc\echo\echo_security mkdir int.msvc\echo\echo_security
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\echo\echo_security.cpp /Foint.msvc\echo\echo_security\ /c
	$(LD) $(LDFLAGS) int.msvc\echo\echo_security\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\echo\echo_security.exe
	-@$(_VC_MANIFEST_EMBED_EXE)
	

echo_samples: echo_dir echo_exe echo_blocking_exe echo_blocking_addr_exe echo_blocking_dual_exe echo_blocking_soap11_exe echo_non_blocking_exe echo_non_blocking_dual_exe echo_rest_exe echo_security_exe


flickr_exe:
	if not exist int.msvc\flickr mkdir int.msvc\flickr
	if not exist $(WSFCPP_SAMPLES_DIR)\flickr mkdir $(WSFCPP_SAMPLES_DIR)\flickr

	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\flickr\flickr.cpp /Foint.msvc\flickr\ /c
	$(LD) $(LDFLAGS) int.msvc\flickr\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\flickr\flickr.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

google_exe:
	@if not exist int.msvc\google mkdir int.msvc\google
	@if not exist $(WSFCPP_SAMPLES_DIR)\google mkdir $(WSFCPP_SAMPLES_DIR)\google
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\google\google.cpp /Foint.msvc\google\ /c
	$(LD) $(LDFLAGS) int.msvc\google\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\google\google.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

math_exe:
	@if not exist int.msvc\math mkdir int.msvc\math
	@if not exist $(WSFCPP_SAMPLES_DIR)\math mkdir $(WSFCPP_SAMPLES_DIR)\math
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\math\math.cpp /Foint.msvc\math\ /c
	$(LD) $(LDFLAGS) int.msvc\math\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\math\math.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

notify_exe:
	@if not exist int.msvc\notify mkdir int.msvc\notify
	@if not exist $(WSFCPP_SAMPLES_DIR)\notify mkdir $(WSFCPP_SAMPLES_DIR)\notify
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\notify\notify.cpp /Foint.msvc\notify\ /c
	$(LD) $(LDFLAGS) int.msvc\notify\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\notify\notify.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

yahoo_exe:
	@if not exist int.msvc\yahoo mkdir int.msvc\yahoo
	@if not exist $(WSFCPP_SAMPLES_DIR)\yahoo mkdir $(WSFCPP_SAMPLES_DIR)\yahoo
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\yahoo\yahoo.cpp /Foint.msvc\yahoo\ /c
	$(LD) $(LDFLAGS) int.msvc\yahoo\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\yahoo\yahoo.exe
	-@$(_VC_MANIFEST_EMBED_EXE)

mtom_exe:
	@if not exist int.msvc\mtom mkdir int.msvc\mtom
	@if not exist $(WSFCPP_SAMPLES_DIR)\mtom mkdir $(WSFCPP_SAMPLES_DIR)\mtom
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(CLIENT_SAMPLES_HOME_DIR)\mtom\mtom.cpp /Foint.msvc\mtom\ /c
	$(LD) $(LDFLAGS) int.msvc\mtom\*.obj $(LIBS) /OUT:$(WSFCPP_SAMPLES_DIR)\mtom\mtom.exe
	-@$(_VC_MANIFEST_EMBED_EXE)
	@xcopy /E /I /Q /Y $(CLIENT_SAMPLES_HOME_DIR)\mtom\resources $(WSFCPP_SAMPLES_DIR)\mtom\resources
	@if exist $(WSFCPP_SAMPLES_DIR)\mtom\resources\*.am del $(WSFCPP_SAMPLES_DIR)\mtom\resources\*.am

echo_service:
	@if not exist int.msvc\services\echo mkdir int.msvc\services\echo
	@if not exist $(WSFCPP_HOME_DIR)\services\echo mkdir $(WSFCPP_HOME_DIR)\services\echo
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(SERVICES_SAMPLES_HOME_DIR)\echo\Echo.cpp /Foint.msvc\services\echo\ /c
	$(LD) $(LDFLAGS) int.msvc\services\echo\*.obj $(LIBS) /DLL /OUT:$(WSFCPP_HOME_DIR)\services\echo\echo.dll
	-@$(_VC_MANIFEST_EMBED_EXE)
	@copy  /Y $(SERVICES_SAMPLES_HOME_DIR)\echo\services.xml $(WSFCPP_HOME_DIR)\services\echo\services.xml

mtom_service:
	@if not exist int.msvc\services\mtom mkdir int.msvc\services\mtom
	@if not exist $(WSFCPP_HOME_DIR)\services\mtom mkdir $(WSFCPP_HOME_DIR)\services\mtom
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(SERVICES_SAMPLES_HOME_DIR)\mtom\*.cpp /Foint.msvc\services\mtom\ /c
	$(LD) $(LDFLAGS) int.msvc\services\mtom\*.obj $(LIBS) /DLL /OUT:$(WSFCPP_HOME_DIR)\services\mtom\mtom.dll
	-@$(_VC_MANIFEST_EMBED_EXE)
	@copy  /Y $(SERVICES_SAMPLES_HOME_DIR)\mtom\services.xml $(WSFCPP_HOME_DIR)\services\mtom\services.xml

wsfcpp_samples: int_dir echo_samples flickr_exe google_exe math_exe notify_exe yahoo_exe mtom_exe echo_service mtom_service


clean: 
	@if exist int.msvc rmdir /s /q int.msvc
		
dist: clean wsfcpp_samples
