#define RootSignatureMain "\
DescriptorTable(CBV(offset = 0, b0, space = 0, numdescriptors = 1)),\
DescriptorTable(CBV(offset = 0, b0, space = 1, numdescriptors = 1)),\
DescriptorTable(SRV(offset = 0, t0, space = 0, numdescriptors = 4)),\
DescriptorTable(Sampler(offset = 0, s0, space = 0, numdescriptors = 1))"

struct IA_TO_VS
{
	//If The Mesh Need To Be Skined, The Skin ComputeShader Pass Will Generate The vPosition And qTangent
	//The GeometryMesh Vertex Shader Remains The Same
	float3 vPosition : Position0;
	snorm float4 qTangent : Tangent0; //R8G8B8A8_SNORM //Input Assembler Vertex Buffer
	
	//The UV
	unorm float2 vUV_Normal : TexCoord0; //R16G16_UNORM  //Input Assembler Vertex Buffer //Texture Width/Height < 65536(==2^16) //It Is Enough
	unorm float2 vUV_Albedo : TexCoord1;
	unorm float2 vUV_ColorSpecular : TexCoord2;
	unorm float2 vUV_Glossiness : TexCoord3;
};

struct VS_TO_RS
{
	float4 vPosition : SV_Position0;
	snorm float4 qTangent : Tangent0;
	unorm float2 vUV_Normal : TexCoord0;
	unorm float2 vUV_Albedo : TexCoord1;
	unorm float2 vUV_ColorSpecular : TexCoord2;
	unorm float2 vUV_Glossiness : TexCoord3;
};

cbuffer g_CBuffer_Sensor:register(b0, space0)
{
	row_major float4x4 g_Sensor_View;
	row_major float4x4 g_Sensor_InverseView;
	row_major float4x4 g_Sensor_Projection;
	row_major float4x4 g_Sensor_InverseProjection;
};

cbuffer g_CBuffer_Mesh:register(b0, space1)
{
	row_major float2x4 g_Mesh_dqWorld;
};

[RootSignature(RootSignatureMain)]
VS_TO_RS VertexMain(IA_TO_VS VertexIn)
{
	//^q = q0 +q1ε = r + ([0, (1/2)×t]×r)ε
	float4 q0 = g_Mesh_dqWorld[0];
	float4 q1 = g_Mesh_dqWorld[1];
	//r = q0
	float4 qRotationWorld = q0;
	//t
	//= 2 * { Quat_Mul(q1,Quat_Con(q0)).xyz } 
	//= 2 * { q0.xyz*(-q1.w) + q1.xyz*q0.w + cross(q0.xyz,q1.xyz) }
	float3 vTranslationWorld = (q0.xyz*(-q1.w) + q1.xyz*q0.w + cross(q0.xyz, q1.xyz))*2.0f;

	//Position In ModelSpace To Quaternion-TangentSpace In WorldSpace
	float3 vPositionModel = VertexIn.vPosition;

	//Rotation:p = p + cross(qRotationWorld.xyz, cross(qRotationWorld.xyz, p) + p.xyz*qRotationWorld.w) *2 
	//Translation:p = p + vTranslation
	float3 vPositionWorld = vPositionModel + cross(qRotationWorld.xyz, cross(qRotationWorld.xyz, vPositionModel) + vPositionModel * qRotationWorld.w)*2.0f + vTranslationWorld;

	//Quaternion-TangentSpace In ModelSpace To Quaternion-TangentSpace In WorldSpace
	float4 qTangent = VertexIn.qTangent;
	
	//Rotation:qRotationWorld×qSkinedTangent = float4(qTangent.xyz×qRotationWorld.w + qRotationWorld.xyz×qTangent.w + cross(qRotationWorld.xyz, qTangent.xyz), qRotationWorld.w·qTangent.w - dot(qRotationWorld.xyz, qTangent,xyz))
	float4 qWorldTangent = float4(qTangent.xyz*qRotationWorld.w + qRotationWorld.xyz*qTangent.w + cross(qRotationWorld.xyz, qTangent.xyz), qRotationWorld.w*qTangent.w - dot(qRotationWorld.xyz, qTangent.xyz));

	VS_TO_RS VertexOut;

	//WorldSpace
	VertexOut.vPosition = float4(vPositionWorld, 1.0f);
	//WiewSpace
	VertexOut.vPosition = mul(VertexOut.vPosition, g_Sensor_View);
	//ProjectionSpace
	VertexOut.vPosition = mul(VertexOut.vPosition, g_Sensor_Projection);

	//WorldSpace
	VertexOut.qTangent = qWorldTangent;

	VertexOut.vUV_Normal = VertexIn.vUV_Normal;
	VertexOut.vUV_Albedo = VertexIn.vUV_Albedo;
	VertexOut.vUV_ColorSpecular = VertexIn.vUV_ColorSpecular;
	VertexOut.vUV_Glossiness = VertexIn.vUV_Glossiness;

	return VertexOut;
}

struct RS_TO_PS
{
	float4 vPosition : SV_Position0;
	snorm float4 qTangent : Tangent0;
	unorm float2 vUV_Normal : TexCoord0;
	unorm float2 vUV_Albedo : TexCoord1;
	unorm float2 vUV_ColorSpecular : TexCoord2;
	unorm float2 vUV_Glossiness : TexCoord3;
};

struct PS_TO_OM
{
	snorm float3 GBuffer_Normal : SV_TARGET0;//snorm float
	float3 GBuffer_Albedo : SV_TARGET1; //HDR Display
	float3 GBuffer_ColorSpecular : SV_TARGET2;
	unorm float GBuffer_Glossiness : SV_TARGET3;
};

Texture2D<snorm float3> g_Texture_Normal:register(t0, space0);
Texture2D<unorm float3> g_Texture_Albedo:register(t1, space0);
Texture2D<unorm float3> g_Texture_ColorSpecular:register(t2, space0);
Texture2D<unorm float> g_Texture_Glossiness:register(t3, space0);

SamplerState g_Sampler:register(s0, space0);

[RootSignature(RootSignatureMain)]
[earlydepthstencil]
PS_TO_OM PixelMain_PBR(RS_TO_PS PixelIn)
{
	float3 vNormal_TangentSpace = normalize(g_Texture_Normal.Sample(g_Sampler, PixelIn.vUV_Normal).xyz); //NormalMap中存储的数据并不一定归一化
	
	float4 qTangent = normalize(PixelIn.qTangent);//光栅化插值后的模不一定仍为1

	//vNormal_WorldSpace = vNormal_TangentSpace + cross(qTangent_v, cross(qTangent_v, vNormal_TangentSpace) + vNormal_TangentSpace.xyz*qTangent_s) * 2
	float3 vNormal_WorldSpace = vNormal_TangentSpace + cross(qTangent.xyz, cross(qTangent.xyz, vNormal_TangentSpace) + vNormal_TangentSpace * qTangent.w)*2.0f; 
	
	PS_TO_OM PixelOut;
	PixelOut.GBuffer_Normal = vNormal_WorldSpace;
	PixelOut.GBuffer_Albedo = g_Texture_Albedo.Sample(g_Sampler, PixelIn.vUV_Albedo);
	PixelOut.GBuffer_ColorSpecular = g_Texture_ColorSpecular.Sample(g_Sampler, PixelIn.vUV_ColorSpecular);
	PixelOut.GBuffer_Glossiness = g_Texture_Glossiness.Sample(g_Sampler, PixelIn.vUV_Glossiness);

	return PixelOut;
}

[RootSignature(RootSignatureMain)]
[earlydepthstencil]
PS_TO_OM PixelMain_SSS(RS_TO_PS PixelIn)
{
	float3 vNormal_TangentSpace = normalize(g_Texture_Normal.Sample(g_Sampler, PixelIn.vUV_Normal).xyz); //NormalMap中存储的数据并不一定归一化

	float4 qTangent = normalize(PixelIn.qTangent);//光栅化插值后的模不一定仍为1

	//vNormal_WorldSpace = vNormal_TangentSpace + cross(qTangent_v, cross(qTangent_v, vNormal_TangentSpace) + vNormal_TangentSpace.xyz*qTangent_s) * 2
	float3 vNormal_WorldSpace = vNormal_TangentSpace + cross(qTangent.xyz, cross(qTangent.xyz, vNormal_TangentSpace) + vNormal_TangentSpace * qTangent.w)*2.0f;

	PS_TO_OM PixelOut;
	PixelOut.GBuffer_Normal = vNormal_WorldSpace;
	PixelOut.GBuffer_Albedo = g_Texture_Albedo.Sample(g_Sampler, PixelIn.vUV_Albedo);
	PixelOut.GBuffer_ColorSpecular = g_Texture_ColorSpecular.Sample(g_Sampler, PixelIn.vUV_ColorSpecular);
	PixelOut.GBuffer_Glossiness = g_Texture_Glossiness.Sample(g_Sampler, PixelIn.vUV_Glossiness);

	return PixelOut;
}