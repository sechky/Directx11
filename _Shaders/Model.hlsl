cbuffer VS_Camera : register(b0)
{
    matrix _world;
    matrix _view;
    matrix _projection;
};

cbuffer PS_Sun : register(b0)
{
    float3 _light;
    float _padding;
}

cbuffer PS_Color : register(b1)
{
    float4 _ambient;
    float4 _diffuse;
    float4 _specular;
    float4 _emissive;
}

struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

PixelInput VS(VertexInput input)
{
    PixelInput output;
    
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = mul(input.normal, (float3x3) _world);
    output.normal = normalize(output.normal);

    output.uv = input.uv;

    return output;
}


SamplerState _samp : register(s0);
Texture2D _map : register(t0);

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = _map.Sample(_samp, input.uv);
    
    return color;
}