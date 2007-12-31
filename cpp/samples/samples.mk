AUTOCONF = .\..\configure.in
!include $(AUTOCONF)

WSFCPP_HOME_DIR="%WSFCPP_HOME%"
SAMPLES_HOME_DIR=.\

CFLAGS = /nologo /w /D "WIN32" /D "_WINDOWS" /D "_MBCS"

LDFLAGS = /nologo /LIBPATH:$(WSFCPP_HOME_DIR)\lib

LIBS = axutil.lib axis2_engine.lib axis2_parser.lib axiom.lib wso2_wsf.lib

INCLUDE_PATH = /I$(WSFCPP_HOME_DIR)\include

!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS) /D "_DEBUG" /Od /Z7 $(CRUNTIME)d
LDFLAGS = $(LDFLAGS) /DEBUG
!else
CFLAGS = $(CFLAGS) /D "NDEBUG" /O2 $(CRUNTIME)
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
             @if not exist "$(WSFCPP_HOME_DIR)\bin\samples\cpp\echo" mkdir "$(WSFCPP_HOME_DIR)\bin\samples\cpp\echo"
             
echo_exe:
             @if not exist int.msvc\echo\echo mkdir int.msvc\echo\echo
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\echo\echo.cpp /Foint.msvc\echo\echo\ /c
	@link.exe $(LDFLAGS) int.msvc\echo\echo\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\echo\echo.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

echo_blocking_exe:
             @if not exist int.msvc\echo\echo_blocking mkdir int.msvc\echo\echo_blocking
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\echo\echo_blocking.cpp /Foint.msvc\echo\echo_blocking\ /c
	@link.exe $(LDFLAGS) int.msvc\echo\echo_blocking\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\echo\echo_blocking.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

echo_blocking_addr_exe:
             @if not exist int.msvc\echo\echo_blocking_addr mkdir int.msvc\echo\echo_blocking_addr
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\echo\echo_blocking_addr.cpp /Foint.msvc\echo\echo_blocking_addr\ /c
	@link.exe $(LDFLAGS) int.msvc\echo\echo_blocking_addr\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\echo\echo_blocking_addr.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

echo_blocking_dual_exe:
             @if not exist int.msvc\echo\echo_blocking_dual mkdir int.msvc\echo\echo_blocking_dual
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\echo\echo_blocking_dual.cpp /Foint.msvc\echo\echo_blocking_dual\ /c
	@link.exe $(LDFLAGS) int.msvc\echo\echo_blocking_dual\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\echo\echo_blocking_dual.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

echo_blocking_soap11_exe:
             @if not exist int.msvc\echo\echo_blocking_soap11 mkdir int.msvc\echo\echo_blocking_soap11
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\echo\echo_blocking_soap11.cpp /Foint.msvc\echo\echo_blocking_soap11\ /c
	@link.exe $(LDFLAGS) int.msvc\echo\echo_blocking_soap11\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\echo\echo_blocking_soap11.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

echo_non_blocking_exe:
             @if not exist int.msvc\echo\echo_non_blocking mkdir int.msvc\echo\echo_non_blocking
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\echo\echo_non_blocking.cpp /Foint.msvc\echo\echo_non_blocking\ /c
	@link.exe $(LDFLAGS) int.msvc\echo\echo_non_blocking\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\echo\echo_non_blocking.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

echo_non_blocking_dual_exe:
             @if not exist int.msvc\echo\echo_non_blocking_dual mkdir int.msvc\echo\echo_non_blocking_dual
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\echo\echo_non_blocking_dual.cpp /Foint.msvc\echo\echo_non_blocking_dual\ /c
	@link.exe $(LDFLAGS) int.msvc\echo\echo_non_blocking_dual\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\echo\echo_non_blocking_dual.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

echo_rest_exe:
             @if not exist int.msvc\echo\echo_rest mkdir int.msvc\echo\echo_rest
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\echo\echo_rest.cpp /Foint.msvc\echo\echo_rest\ /c
	@link.exe $(LDFLAGS) int.msvc\echo\echo_rest\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\echo\echo_rest.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

echo_security_exe:
             @if not exist int.msvc\echo\echo_security mkdir int.msvc\echo\echo_security
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\echo\echo_security.cpp /Foint.msvc\echo\echo_security\ /c
	@link.exe $(LDFLAGS) int.msvc\echo\echo_security\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\echo\echo_security.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

echo_samples: echo_dir echo_exe echo_blocking_exe echo_blocking_addr_exe echo_blocking_dual_exe echo_blocking_soap11_exe echo_non_blocking_exe echo_non_blocking_dual_exe echo_rest_exe echo_security_exe

flickr_exe:
             @if not exist int.msvc\flickr mkdir int.msvc\flickr
             @if not exist "$(WSFCPP_HOME_DIR)\bin\samples\cpp\flickr" mkdir "$(WSFCPP_HOME_DIR)\bin\samples\cpp\flickr"
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\flickr\flickr.cpp /Foint.msvc\flickr\ /c
	@link.exe $(LDFLAGS) int.msvc\flickr\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\flickr\flickr.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

google_exe:
             @if not exist int.msvc\google mkdir int.msvc\google
             @if not exist "$(WSFCPP_HOME_DIR)\bin\samples\cpp\google" mkdir "$(WSFCPP_HOME_DIR)\bin\samples\cpp\google"
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\google\google.cpp /Foint.msvc\google\ /c
	@link.exe $(LDFLAGS) int.msvc\google\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\google\google.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

math_exe:
             @if not exist int.msvc\math mkdir int.msvc\math
             @if not exist "$(WSFCPP_HOME_DIR)\bin\samples\cpp\math" mkdir "$(WSFCPP_HOME_DIR)\bin\samples\cpp\math"
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\math\math.cpp /Foint.msvc\math\ /c
	@link.exe $(LDFLAGS) int.msvc\math\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\math\math.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

notify_exe:
             @if not exist int.msvc\notify mkdir int.msvc\notify
             @if not exist "$(WSFCPP_HOME_DIR)\bin\samples\cpp\notify" mkdir "$(WSFCPP_HOME_DIR)\bin\samples\cpp\notify"
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\notify\notify.cpp /Foint.msvc\notify\ /c
	@link.exe $(LDFLAGS) int.msvc\notify\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\notify\notify.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

yahoo_exe:
             @if not exist int.msvc\yahoo mkdir int.msvc\yahoo
             @if not exist "$(WSFCPP_HOME_DIR)\bin\samples\cpp\yahoo" mkdir "$(WSFCPP_HOME_DIR)\bin\samples\cpp\yahoo"
	@cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\yahoo\yahoo.cpp /Foint.msvc\yahoo\ /c
	@link.exe $(LDFLAGS) int.msvc\yahoo\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\yahoo\yahoo.exe
             -@$(_VC_MANIFEST_EMBED_EXE)

mtom_exe:
             @if not exist int.msvc\mtom mkdir int.msvc\mtom
             @if not exist "$(WSFCPP_HOME_DIR)\bin\samples\cpp\mtom" mkdir "$(WSFCPP_HOME_DIR)\bin\samples\cpp\mtom"
             @cl.exe $(CFLAGS) $(INCLUDE_PATH) $(SAMPLES_HOME_DIR)\mtom\mtom.cpp /Foint.msvc\mtom\ /c
	@link.exe $(LDFLAGS) int.msvc\mtom\*.obj $(LIBS) /OUT:$(WSFCPP_HOME_DIR)\bin\samples\cpp\mtom\mtom.exe
             -@$(_VC_MANIFEST_EMBED_EXE)
             @xcopy /E /I /Q /Y $(SAMPLES_HOME_DIR)\mtom\resources $(WSFCPP_HOME_DIR)\bin\samples\cpp\mtom\resources
             @del $(WSFCPP_HOME_DIR)\bin\samples\cpp\mtom\resources\*.am

wsfcpp_samples: int_dir echo_samples flickr_exe google_exe math_exe notify_exe yahoo_exe mtom_exe

clean: 
	@if exist int.msvc rmdir /s /q int.msvc
		
dist: clean wsfcpp_samples
