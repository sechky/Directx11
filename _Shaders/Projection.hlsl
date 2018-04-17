#include "Constants.hlsl"

cbuffer ProjectionBuffer
{
    matrix _view2;
    matrix _projection2;
}

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;

    float4 viewPosition : TEXCOORD1;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ProjectionVertexShader(VertexInputType input)
{
    PixelInputType output;
    

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);


    // Store the position of the vertice as viewed by the projection view point in a separate variable.
    output.viewPosition = mul(input.position, _world);
    output.viewPosition = mul(output.viewPosition, _view2);
    output.viewPosition = mul(output.viewPosition, _projection2);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3) _world);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

    return output;
}

Texture2D _shaderTexture : register(t0);
Texture2D _projectionTexture : register(t1);

SamplerState _sampleType;



////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ProjectionPixelShader(PixelInputType input) : SV_TARGET
{
    float4 color;
    float3 lightDir;
    float lightIntensity;
    float4 textureColor;
    float2 projectTexCoord;
    float4 projectionColor;

    //// Set the default output color to the ambient light value for all pixels.
    //color = ambientColor;

    //// Invert the light direction for calculations.
    //lightDir = -lightDirection;

    //// Calculate the amount of light on this pixel.
    //lightIntensity = saturate(dot(input.normal, lightDir));

    //if (lightIntensity > 0.0f)
    //{
    //    // Determine the light color based on the diffuse color and the amount of light intensity.
    //    color += (diffuseColor * lightIntensity);
    //}

    // Saturate the light color.
    color = saturate(color);

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = _shaderTexture.Sample(_sampleType, input.tex);

    // Combine the light color and the texture color.
    color = color * textureColor;

	// Calculate the projected texture coordinates.
    projectTexCoord.x = input.viewPosition.x / input.viewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.viewPosition.y / input.viewPosition.w / 2.0f + 0.5f;

    // Determine if the projected coordinates are in the 0 to 1 range.  If it is then this pixel is inside the projected view port.
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        //Sample the projection texture using the projected texture coordinates and then set the output pixel color to be the projected texture color.
		

        // Sample the color value from the projection texture using the sampler at the projected texture coordinate location.
        projectionColor = _projectionTexture.Sample(_sampleType, projectTexCoord);

        // Set the output color of this pixel to the projection texture overriding the regular color value.
        color = projectionColor;
    }

    return color;

}
