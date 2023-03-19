cd /D D:\Visual Studio 2022 Projects\GameSaver\GameSaver\shaders

D:\VulkanSDK\1.3.236.0\Bin\glslc.exe -fshader-stage=vertex VertexShader.hlsl -c -o VertexShader.spv
D:\VulkanSDK\1.3.236.0\Bin\glslc.exe -fshader-stage=fragment PixelShader.hlsl -c -o PixelShader.spv
