#version 310 es
#extension GL_EXT_tessellation_shader : enable
#extension GL_OES_tessellation_shader : enable

//glPatchParameteriOES(GL_PATCH_VERTICES_OES, 2)

layout(location = 6) in vec2 VS_Patch_PositionTangentXY[];
layout(location = 7) in vec2 VS_Patch_UV[];

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
	highp vec4 g_FrustumPlane[6];
};

layout(row_major, binding = 4) uniform g_BufferConstant_TerrainTile
{
    //Tx Bx Nx Ox
	//Ty By Ny Oy
	//Tz Bz Nz Oz
	//0  0  0  1
	highp mat4x4 g_TangentToWorld;//基变换 Tangent Bitangent Normal 平移变换 Origin
};

layout(binding = 10) uniform highp sampler2D g_TextureHeightMaxMin;

layout(location = 6) out vec2 DS_Patch_PositionTangentXY[];
layout(location = 7) out vec2 DS_Patch_UV[];

layout(vertices = 2) out; //Output Patch Control Point

void main()
{
    DS_Patch_PositionTangentXY[gl_InvocationID] = VS_Patch_PositionTangentXY[gl_InvocationID];
    DS_Patch_UV[gl_InvocationID] = VS_Patch_UV[gl_InvocationID];

	//Patch Constant Function   
    barrier();

	//Frustum Cull
	vec2 l_PatchCenter_UV;
	l_PatchCenter_UV.x = (VS_Patch_UV[0].x + VS_Patch_UV[1].x) * 0.5f;
	l_PatchCenter_UV.y = (VS_Patch_UV[0].y + VS_Patch_UV[1].y) * 0.5f;

	vec2 l_Patch_Displacement_MaxMin = texture(g_TextureHeightMaxMin, l_PatchCenter_UV).rg * 30.0f;

	vec3 l_Patch_AABB_Point_PositionTangent[8]; 
	l_Patch_AABB_Point_PositionTangent[0] = vec3(VS_Patch_PositionTangentXY[0].x, VS_Patch_PositionTangentXY[0].y, l_Patch_Displacement_MaxMin.x);
	l_Patch_AABB_Point_PositionTangent[1] = vec3(VS_Patch_PositionTangentXY[0].x, VS_Patch_PositionTangentXY[1].y, l_Patch_Displacement_MaxMin.x);
	l_Patch_AABB_Point_PositionTangent[2] = vec3(VS_Patch_PositionTangentXY[1].x, VS_Patch_PositionTangentXY[1].y, l_Patch_Displacement_MaxMin.x);
	l_Patch_AABB_Point_PositionTangent[3] = vec3(VS_Patch_PositionTangentXY[1].x, VS_Patch_PositionTangentXY[0].y, l_Patch_Displacement_MaxMin.x);
	l_Patch_AABB_Point_PositionTangent[4] = vec3(VS_Patch_PositionTangentXY[0].x, VS_Patch_PositionTangentXY[0].y, l_Patch_Displacement_MaxMin.y);
	l_Patch_AABB_Point_PositionTangent[5] = vec3(VS_Patch_PositionTangentXY[0].x, VS_Patch_PositionTangentXY[1].y, l_Patch_Displacement_MaxMin.y);
	l_Patch_AABB_Point_PositionTangent[6] = vec3(VS_Patch_PositionTangentXY[1].x, VS_Patch_PositionTangentXY[1].y, l_Patch_Displacement_MaxMin.y);
	l_Patch_AABB_Point_PositionTangent[7] = vec3(VS_Patch_PositionTangentXY[1].x, VS_Patch_PositionTangentXY[0].y, l_Patch_Displacement_MaxMin.y);

	vec3 l_Patch_AABB_Point_PositionWorld[8]; 
	for(int i = 0; i < 8; ++i)
	{
		l_Patch_AABB_Point_PositionWorld[i] = (g_TangentToWorld * vec4(l_Patch_AABB_Point_PositionTangent[i], 1.0f)).xyz;
	}

	vec3 l_Patch_AABB_World_Max = max(
		max(
			max(l_Patch_AABB_Point_PositionWorld[0], l_Patch_AABB_Point_PositionWorld[1]),
			max(l_Patch_AABB_Point_PositionWorld[2], l_Patch_AABB_Point_PositionWorld[3])
		),
		max(
			max(l_Patch_AABB_Point_PositionWorld[4], l_Patch_AABB_Point_PositionWorld[5]),
			max(l_Patch_AABB_Point_PositionWorld[6], l_Patch_AABB_Point_PositionWorld[7])
		)
	);

	vec3 l_Patch_AABB_World_Min = min(
		min(
			min(l_Patch_AABB_Point_PositionWorld[0], l_Patch_AABB_Point_PositionWorld[1]),
			min(l_Patch_AABB_Point_PositionWorld[2], l_Patch_AABB_Point_PositionWorld[3])
		),
		min(
			min(l_Patch_AABB_Point_PositionWorld[4], l_Patch_AABB_Point_PositionWorld[5]),
			min(l_Patch_AABB_Point_PositionWorld[6], l_Patch_AABB_Point_PositionWorld[7])
		)
	);

	vec3 l_Patch_AABB_World_Center = (l_Patch_AABB_World_Max + l_Patch_AABB_World_Min) * 0.5f;
	vec3 l_Patch_AABB_World_HalfDiagonal = (l_Patch_AABB_World_Max - l_Patch_AABB_World_Min) * 0.5f;

	bool l_InVisible = false;
	for(int i = 0; i < 6; ++i)
	{
		//长方体中心到平面上任意一点在平面法向量上的投影
		float l_Distance = dot(g_FrustumPlane[i], vec4(l_Patch_AABB_World_Center,1.0f));
		
		//长方体8个半对角线在平面法向量上的投影的最大值
		//映射
		//逐分量相乘
		vec3 l_Temp =  l_Patch_AABB_World_HalfDiagonal.xyz * g_FrustumPlane[i].xyz;
		//取绝对值
		l_Temp = abs(l_Temp);
		//归约
		float l_HalfDiagonalProjection = l_Temp.x + l_Temp.y + l_Temp.z;

		if(l_Distance < -l_HalfDiagonalProjection)
		{
			l_InVisible = true;
			break;
		}
	}

	if(l_InVisible)
	{
		gl_TessLevelOuter[0] = -1.0f;
		gl_TessLevelOuter[1] = -1.0f;
		gl_TessLevelOuter[2] = -1.0f;
		gl_TessLevelOuter[3] = -1.0f;
		gl_TessLevelInner[0] = -1.0f;
		gl_TessLevelInner[1] = -1.0f;
	}
	else
	{
		//Level Of Detail

		//极差->反映Density
		vec2 l_Patch_Displacement_MaxMin_R = textureOffset(g_TextureHeightMaxMin, l_PatchCenter_UV, ivec2(1,0)).rg * 30.0f;
		vec2 l_Patch_Displacement_MaxMin_T = textureOffset(g_TextureHeightMaxMin, l_PatchCenter_UV, ivec2(0,1)).rg * 30.0f;
		vec2 l_Patch_Displacement_MaxMin_L = textureOffset(g_TextureHeightMaxMin, l_PatchCenter_UV, ivec2(-1,0)).rg * 30.0f;
		vec2 l_Patch_Displacement_MaxMin_B = textureOffset(g_TextureHeightMaxMin, l_PatchCenter_UV, ivec2(0,-1)).rg * 30.0f;

		float l_Patch_Range_M = l_Patch_Displacement_MaxMin.x - l_Patch_Displacement_MaxMin.y;
		float l_Patch_Range_R = l_Patch_Displacement_MaxMin_R.x - l_Patch_Displacement_MaxMin_R.y;
		float l_Patch_Range_T = l_Patch_Displacement_MaxMin_T.x - l_Patch_Displacement_MaxMin_T.y;
		float l_Patch_Range_L = l_Patch_Displacement_MaxMin_L.x - l_Patch_Displacement_MaxMin_L.y;
		float l_Patch_Range_B = l_Patch_Displacement_MaxMin_B.x - l_Patch_Displacement_MaxMin_B.y;

		float l_Range[4];
		l_Range[0] = (l_Patch_Range_M + l_Patch_Range_L) * 0.5f;
		l_Range[1] = (l_Patch_Range_M + l_Patch_Range_B) * 0.5f;
		l_Range[2] = (l_Patch_Range_M + l_Patch_Range_R) * 0.5f;
		l_Range[3] = (l_Patch_Range_M + l_Patch_Range_T) * 0.5f;

		//投影->反映Distance
		vec3 l_Patch_Point_PositionTangent[4];
		l_Patch_Point_PositionTangent[0] = vec3(VS_Patch_PositionTangentXY[0].x, VS_Patch_PositionTangentXY[1].y, 0.0f);
		l_Patch_Point_PositionTangent[1] = vec3(VS_Patch_PositionTangentXY[0].x, VS_Patch_PositionTangentXY[0].y, 0.0f);
		l_Patch_Point_PositionTangent[2] = vec3(VS_Patch_PositionTangentXY[1].x, VS_Patch_PositionTangentXY[0].y, 0.0f);
		l_Patch_Point_PositionTangent[3] = vec3(VS_Patch_PositionTangentXY[1].x, VS_Patch_PositionTangentXY[1].y, 0.0f);

		vec3 l_Patch_Point_PositionView[5];
		for(int i = 0; i < 4; ++i)
		{
			l_Patch_Point_PositionView[i] = (g_View * g_TangentToWorld * vec4(l_Patch_Point_PositionTangent[i], 1.0f)).xyz;
		}
		l_Patch_Point_PositionView[4] = l_Patch_Point_PositionView[0];
		
		float l_Diameter_Projection[4];
		for(int i = 0; i < 4; ++i)
		{
			vec3 l_Center = (l_Patch_Point_PositionView[i] + l_Patch_Point_PositionView[i+1]) * 0.5f;
			vec3 l_SensorToCenter = l_Center;//l_Center - 0,0,0
			//vec3 l_ViewDirection = vec3(0.0f,0.0f,-1.0f);
			//float l_Distance = abs(dot(l_SensorToCenter,l_ViewDirection));//较近的Edge的Center可能比NearZ平面更近，从而为负值
			float l_Distance = abs(l_SensorToCenter.z);
			float l_Diameter = length(l_Patch_Point_PositionView[i] - l_Patch_Point_PositionView[i+1]);
			l_Diameter_Projection[i] =  l_Diameter * (1.0f / l_Distance);
		}


		gl_TessLevelOuter[0] = pow(max(l_Range[0],1.0f),1.25f)*pow(l_Diameter_Projection[0],1.25f)*8.0f; //l_Range至少为1.0，防止绝对平的地形被剔除 //5.0f = 1.0f/2.0f(NormalizedDevice)*800.0f(ScreenSize)/80.0f(TesselationTriangleSize)
		gl_TessLevelOuter[1] = pow(max(l_Range[1],1.0f),1.25f)*pow(l_Diameter_Projection[1],1.25f)*8.0f;
		gl_TessLevelOuter[2] = pow(max(l_Range[2],1.0f),1.25f)*pow(l_Diameter_Projection[2],1.25f)*8.0f;
		gl_TessLevelOuter[3] = pow(max(l_Range[3],1.0f),1.25f)*pow(l_Diameter_Projection[3],1.25f)*8.0f;
		gl_TessLevelInner[0] = (gl_TessLevelOuter[1] + gl_TessLevelOuter[3]) * 0.5f;
		gl_TessLevelInner[1] = (gl_TessLevelOuter[0] + gl_TessLevelOuter[2]) * 0.5f;
	}
}