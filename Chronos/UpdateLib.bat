

xcopy	/y		.\Engine\Bin\Engine.lib					.\EngineSDK\Lib\
xcopy	/y		.\Engine\Bin\Engine.dll					.\Client\Bin\
xcopy	/y/s		.\Engine\Public\*.*					.\EngineSDK\Inc\

xcopy	/y		.\Engine\FmodLowLevel\lib\*.lib				.\EngineSDK\Lib\
xcopy	/y		.\Engine\FmodLowLevel\lib\*.dll				.\Client\Bin\
xcopy	/y		.\Engine\FmodLowLevel\Inc\*					.\EngineSDK\Inc\