//GLOBALS

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

//TYPEDEFS
struct VertexInputType
{
	float4 position : POSITION;
	//float3 instancePosition : TEXCOORD;
};


struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
};

//VERTEX SHADER
PixelInputType DepthVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f;
	/*
	// Update the position of the vertices based on the data for this particular instance.
    input.position.x += input.instancePosition.x;
    input.position.y += input.instancePosition.y;
    input.position.z += input.instancePosition.z;
	*/
	//calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.depthPosition = output.position;

	return output;
}