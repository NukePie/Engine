//GLOBALS
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer FogBuffer
{
	float fogStart;
	float fogEnd;
};

//TYPEDEFS
struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 color : COLOR;
	float3 camPosition : NORMAL1;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 pixelPosition : COLOR0;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL0;
	float4 color : COLOR1;
	float3 camPos : NORMAL1;
	float3 viewDir : TEXCOORD1;
	matrix viewMatrix : MATRIX0;
	float3 norm : POSITION0;
	float fogFactor : FOG;
};



//VERTEX SHADER
PixelInputType TerrainVertexShader(VertexInputType input)
{
	PixelInputType output;
	float4 cameraPosition;
	
	output.viewMatrix = worldMatrix;

	input.position.w = 1.0f;


	//calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//store the texture coordinates for the pixel shader
	output.tex = input.tex;

	// Calculate the camera position.
	cameraPosition = mul(input.position, worldMatrix);
	cameraPosition = mul(cameraPosition, viewMatrix);

	// Calculate linear fog.
	output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));

	//calculate the normal vector against the world matrix only.
	output.normal = mul(input.normal, worldMatrix);

	output.normal = normalize(output.normal);


	matrix matr = transpose(viewMatrix);
	
	output.norm = input.normal;

	output.pixelPosition = input.position;

	output.camPos = input.camPosition;

	//send the color map color into the pixelshader
	output.color = input.color;

	return output;
}

	



