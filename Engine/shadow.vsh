//GLOBALS

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

cbuffer LightPositionBuffer
{
	float3 lightPosition;
	float padding;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
};

//TYPEDEFS
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL0;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
	float3 instancePosition : NORMAL1;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
	float4 wPos : TEXCOORD3;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD4;
};

//VERTEX SHADER
PixelInputType ShadowVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.position.w = 1.0f; //4 units for matrix calculations

	input.position.x += input.instancePosition.x;
	input.position.y += input.instancePosition.y;
	input.position.z += input.instancePosition.z;

	//calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.lightViewPosition = mul(input.position, worldMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
	output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

	output.tex = input.tex;

	//calculate the normal vector against the world matrix only.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);

	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);


	output.wPos = mul(input.position, worldMatrix);

	//determine the view direction (for specular lightning)
	output.viewDirection = cameraPosition.xyz - output.wPos.xyz;

	//normalize the direction
	output.viewDirection = normalize(output.viewDirection);

	output.lightPos = mul(lightPosition, worldMatrix);

	return output;
}