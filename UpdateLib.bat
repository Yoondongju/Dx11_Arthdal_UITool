

xcopy	/y		.\Engine\Bin\Engine.lib					.\EngineSDK\Lib\
xcopy	/y		.\Engine\ThirdPartyLib\*.lib			.\EngineSDK\Lib\

xcopy	/y		.\Engine\Bin\Engine.dll					.\Client\Bin\
xcopy	/y		.\Engine\Bin\ShaderFiles\*.*			.\Client\Bin\ShaderFiles\
xcopy	/y/s	.\Engine\Public\*.*						.\EngineSDK\Inc\