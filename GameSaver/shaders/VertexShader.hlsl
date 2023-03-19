
struct VSInput
{
    [[vk::location(0)]] float2 position : POSITION0;
    [[vk::location(1)]] float3 color : COLOR0;
};

struct VSOutput
{
    float4 Pos : SV_POSITION;
    [[vk::location(0)]] float3 Color : COLOR0;
};

[[vk::push_constant]]
cbuffer push
{
    float2 offset;
    float3 color;
};


VSOutput main(VSInput input : POSITION) : SV_POSITION
{
    VSOutput output = (VSOutput) 0;
    
    output.Color = color;
    output.Pos = float4(input.position.xy + offset, 0, 1);
    
    return output;

}