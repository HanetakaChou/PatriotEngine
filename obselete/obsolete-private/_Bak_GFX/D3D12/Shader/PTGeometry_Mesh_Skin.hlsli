//1.DualQuaternion Skin
//Ladislav Kavan, Steven Collins, Jiri Zara, Carol O'Sullivan. "Skinning with Dual Quaternions." I3D 2007.
//http://www.cs.utah.edu/~ladislav/kavan07skinning/kavan07skinning.html
//Ladislav Kavan, Steven Collins, Jiri Zara, Carol O'Sullivan. "Geometric Skinning with Approximate Dual Quaternion Blending." SIGGRAPH 2008.
//http://www.cs.utah.edu/~ladislav/kavan08geometric/kavan08geometric.html
//NVIDIA Direct3D SDK 10.5 Code Samples / Skinning with Dual Quaternions
//https://developer.download.nvidia.com/SDK/10.5/direct3d/samples.html#QuaternionSkinning

//2.Quaternion Tangent Space
//Ivo Zoltan Frey. "Spherical Skinning withDual-Quaternions and Qtangents".SIGGRAPH 2011.
//http://crytek.com/cryengine/presentations/spherical-skinning-with-dual-quaternions-and-qtangents

#define RootSignatureMain "\
DescriptorTable(SRV(offset=0,t0,space=0,numdescriptors=4)),\
DescriptorTable(SRV(offset=0,t0,space=1,numdescriptors=2)),\
DescriptorTable(UAV(offset=0,u0,space=0,numdescriptors=2))"

Buffer<float4> g_Pose_dqTR_0 : register(t0, space0); // R32G32B32A32_FLOAT Buffer
Buffer<float4> g_Pose_dqTR_1: register(t1, space0); // R32G32B32A32_FLOAT Buffer
Buffer<uint4> g_Vertex_vJointIndex: register(t2, space0);  // R8G8B8A8_UINT Buffer
Buffer<unorm float4> g_Vertex_vJointWeight: register(t3, space0); // R8G8B8A8_UNORM Buffer

Buffer<float4> g_Vertex_vPosition_Input: register(t0, space1); // R32G32B32A32_FLOAT Buffer
Buffer<snorm float4> g_Vertex_qTangent_Input: register(t1, space1); // R8G8B8A8_SNORM Buffer

RWBuffer<float4> g_Vertex_vPosition_Output:register(u0, space0); // R32G32B32A32_FLOAT Typed UAV Store
RWBuffer<snorm float4> g_Vertex_qTangent_Output:register(u1, space0); // R8G8B8A8_SNORM Typed UAV Store

[RootSignature(RootSignatureMain)]
[numthreads(64, 1, 1)]
void main(
	uint3 l_LocalID : SV_GroupThreadID,
	uint3 l_GroupID : SV_GroupID,
	uint3 l_GlobalID : SV_DispatchThreadID
)
{
	uint VertexID = l_GlobalID.x;

	//DLB(DualQuaternion Linear Blend)
	float2x4 dqTR;
	{
		uint4 Vertex_vJointIndex = g_Vertex_vJointIndex[VertexID];
		unorm float4 Vertex_vJointWeight = g_Vertex_vJointWeight[VertexID];

		float2x4 dq0 = float2x4(g_Pose_dqTR_0[Vertex_vJointIndex.x], g_Pose_dqTR_1[Vertex_vJointIndex.x]);
		float2x4 dq1 = float2x4(g_Pose_dqTR_0[Vertex_vJointIndex.y], g_Pose_dqTR_1[Vertex_vJointIndex.y]);
		float2x4 dq2 = float2x4(g_Pose_dqTR_0[Vertex_vJointIndex.z], g_Pose_dqTR_1[Vertex_vJointIndex.z]);
		float2x4 dq3 = float2x4(g_Pose_dqTR_0[Vertex_vJointIndex.w], g_Pose_dqTR_1[Vertex_vJointIndex.w]);

		float w0 = Vertex_vJointWeight.x;
		float w1 = Vertex_vJointWeight.y; // (dot(dq1[0], dq0[0]) >= 0.0f) ? Vertex_vJointWeight.y : -Vertex_vJointWeight.y;
		float w2 = Vertex_vJointWeight.z; // (dot(dq2[0], dq0[0]) >= 0.0f) ? Vertex_vJointWeight.z : -Vertex_vJointWeight.z;
		float w3 = Vertex_vJointWeight.w; // (dot(dq3[0], dq0[0]) >= 0.0f) ? Vertex_vJointWeight.w : -Vertex_vJointWeight.w;

		dqTR = dq0 * w0 + dq1 * w1 + dq2 * w2 + dq3 * w3;
	}

	//To Skined ModelSpace
	float3 vSkinedPosition;
	snorm float4 qSkinedTangent;
	{
		//^b = b0 + b1ε
		float4 b0 = dqTR[0];
		float4 b1 = dqTR[1];

		//|b0|^2
		float b0_norm_square = dot(b0, b0);

		//r = b0/|b0|
		float4 qRotation = b0 / sqrt(b0_norm_square);

		//t 
		//= 2 * { [Quat_Mul(b1,Quat_Con(b0))/(|b0|^2)] - [dot(b0.xyzw,b1.xyzw)/(|b0|^2)] }
		//= 2 * { Quat_Mul(b1,Quat_Con(b0)) - dot(b0.xyzw,b1.xyzw) } / (|b0|^2)
		//= 2 * { b0.xyz*(-b1.w) + b1.xyz*b0.w + cross(b0.xyz,b1.xyz) } / (|b0|^2)
		float3 vTranslation = ((b0.xyz*(-b1.w) + b1.xyz*b0.w + cross(b0.xyz, b1.xyz)) / b0_norm_square)*2.0f;

		//Skined Position In ModelSpace

		//p
		float3 p = g_Vertex_vPosition_Input[VertexID].xyz;

		//Rotation:p = p + cross(qRotation_v, cross(qRotation_v, p) + p.xyz*qRotation_s) *2 
		//Translation:p = p + vTranslation
		vSkinedPosition = p + cross(qRotation.xyz, cross(qRotation.xyz, p) + p * qRotation.w)*2.0f + vTranslation;

		//Skined Quaternion-TangentSpace In ModelSpace

#if 0
		//Traditional To Quaternion-TangentSpace
		
		//float3 N = Vertex_vNormal.xyz;
		//float3 T = Vertex_vTangent.xyz;
		//float3 B = cross(N, T); //左手系/右手系都适用

		//QTangent
		//|T.x B.x N.x|
		//|T.y B.y N.y|
		//|T.z B.z N.z|

		//float4 qTangent;
		//qTangent.w = sqrt(T.x + B.y + N.z + 1.0f)*0.5f; //也可以是-0.5f
		//qTangent.x = ((B.z - N.y) / qTangent.w)*0.25f;
		//qTangent.y = ((N.x - T.z) / qTangent.w)*0.25f;
		//qTangent.z = ((T.y - B.x) / qTangent.w)*0.25f;
#endif
		float4 qTangent = g_Vertex_qTangent_Output[VertexID].xyzw;

		//Rotation:qTangent = qRotation×qTangent = float4(qTangent.xyz×qRotation.w + qRotation.xyz×qTangent.w + cross(qRotation.xyz, qTangent.xyz), qRotation.w·qTangent.w - dot(qRotation.xyz, qTangent,xyz))
		qSkinedTangent = float4(qTangent.xyz*qRotation.w + qRotation.xyz*qTangent.w + cross(qRotation.xyz, qTangent.xyz), qRotation.w*qTangent.w - dot(qRotation.xyz, qTangent.xyz));

#if 0
		//Quaternion-TangentSpace In ModelSpace To Quaternion-TangentSpace In WorldSpace

		//WorldTransform的表示可以考虑用DualQuaternion代替Matrix(g_mWorld)
		//float4 qWorld;
		//qWorld.w = sqrt(g_mWorld._m00 + g_mWorld._m11 + g_mWorld._m22 + 1.0f)*0.5f; //也可以是-0.5f
		//qWorld.x = ((g_mWorld._m21 - g_mWorld._m12) / qWorld.w)*0.25f;
		//qWorld.y = ((g_mWorld._m02 - g_mWorld._m20) / qWorld.w)*0.25f;
		//qWorld.z = ((g_mWorld._m10 - g_mWorld._m01) / qWorld.w)*0.25f;

		//Rotation:qWorld×qSkinedTangent = float4(qSkinedTangent.xyz×qWorld.w + qWorld.xyz×qSkinedTangent.w + cross(qWorld.xyz, qSkinedTangent.xyz), qWorld.w·qSkinedTangent.w - dot(qWorld.xyz, qSkinedTangent,xyz))
		//float4 qWorldTangent = float4(qSkinedTangent.xyz*qWorld.w + qWorld.xyz*qSkinedTangent.w + cross(qWorld.xyz, qSkinedTangent.xyz), qWorld.w*qSkinedTangent.w - dot(qWorld.xyz, qSkinedTangent.xyz));
#endif

#if 0
		//Normal TangentSpace To WorldSpace

		//float3 vNormal_TangentSpace = g_NormalMap.Sample(......).xyz;
		//vNormal_TangentSpace = vNormal_TangentSpace.xyz*2.0f - float3(1.0f, 1.0f, 1.0f); //使用SNORM类型，则可以省去此步骤
		//vNormal_TangentSpace = normalize(vNormal_TangentSpace); //NormalMap中存储的数据并不一定归一化

		//vNormal_WorldSpace = vNormal_TangentSpace + cross(qTangent_v, cross(qTangent_v, vNormal_TangentSpace) + vNormal_TangentSpace.xyz*qTangent_s)*2
		//float3 vNormal_WorldSpace = vNormal_TangentSpace + cross(qTangent.xyz, cross(qTangent.xyz, vNormal_TangentSpace) + vNormal_TangentSpace * qTangent.w)*2.0f; 
#endif

	}

	g_Vertex_vPosition_Output[VertexID] = float4(vSkinedPosition, 1.0f);
	g_Vertex_qTangent_Output[VertexID]= qSkinedTangent;
}