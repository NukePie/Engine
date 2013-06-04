//GLOBALS

//C buffers
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer LightPositionBuffer
{
	float4 lightPosition;
};


//TYPEDEFS
struct VertexInputType
{
	float4 position		: POSITION;
	float3 normal		: NORMAL;
	float2 tex			: TEXCOORD0;

};

struct PixelInputType
{
	float4 position		: SV_POSITION;
	float3 worldNormal	: TEXCOORD0;
	float3 lightVec		: TEXCOORD1;
};

//VERTEX SHADER
PixelInputType GoochVertexShader(VertexInputType input)
{
	PixelInputType OUT;
	input.position.w = 1.0f;

	float4 wNormal			= float4(input.normal, 0);
	OUT.worldNormal			= mul(wNormal, worldMatrix).xyz;
	float4 worldPosition	= mul(input.position, worldMatrix); 
	OUT.lightVec			= (lightPosition.xyz - worldPosition.xyz);
	OUT.position			= mul(worldPosition, viewMatrix);
	OUT.position			= mul(OUT.position, projectionMatrix);

	return OUT;
}