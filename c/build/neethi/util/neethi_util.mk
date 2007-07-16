AXIS2_BIN_DIR= ..\..\..\build\axis2c-bin-1.1.0-win32

neethi_util: copy_include
	@cl.exe /nologo /D "WIN32" /D "AXIS2_DECLARE_EXPORT" /D "_WINDOWS" /D "_MBCS" *.C /I$(AXIS2_BIN_DIR)\include /c
	@link.exe /nologo *.obj /LIBPATH:$(AXIS2_BIN_DIR)\lib axiom.lib axutil.lib \
	axis2_engine.lib axis2_parser.lib /DLL /OUT:$(AXIS2_BIN_DIR)\lib\neethi_util.dll \
	/IMPLIB:$(AXIS2_BIN_DIR)\lib\neethi_util.lib


copy_include :
	@copy /Y neethi_options.h .\..\..\..\include\

