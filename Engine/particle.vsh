///Globals//
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

///definitions
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
};

struct PixelInputType //Pixel Shader only reacts to SV_POSITION for position
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
};

////Vertex Shader
PixelInputType ParticleVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f; //we only use X,Y and Z but W Is set to one so when we do matrix calculations it does what we want.

	//Calculate the position of the vertex against the world, view and projection matrices
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;//Store the input texture coordinates for the pixel shader
	output.color = input.color;

	return output; //Send the input coordinates away!
}
