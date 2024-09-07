

xcopy	/y		.\..\Bin\Engine.lib					.\..\..\EngineSDK\Lib\
xcopy	/y		.\..\Bin\Engine.dll					.\..\..\Client\Bin\
xcopy	/y		.\..\Bin\ShaderFiles\*.*			.\..\..\Client\Bin\ShaderFiles\
xcopy	/y/s	.\..\Public\*.*						.\..\..\EngineSDK\Inc\

xcopy	/y		.\..\FmodLowLevel\lib\*.lib			.\..\..\EngineSDK\Lib\
xcopy	/y		.\..\FmodLowLevel\lib\*.dll			.\..\..\Client\Bin\
xcopy	/y		.\..\FmodLowLevel\Inc\*				.\..\..\EngineSDK\Inc\