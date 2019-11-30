#version 310 es
#extension GL_OES_shader_multisample_interpolation : enable

layout(binding = 4) uniform sampler2D g_NormalMap_LowFreq;
layout(binding = 5) uniform sampler2D g_NormalMap_HighFreq;

layout(location = 5) in sample highp vec3 RS_Tangent;
layout(location = 6) in sample highp vec3 RS_Bitangent;
layout(location = 7) in sample highp vec2 RS_UV_NormalMap_LowFreq;
layout(location = 8) in sample highp vec2 RS_UV_NormalMap_HighFreq;

layout(location = 10) in sample highp vec3 RS_PositionWorld;

layout(binding = 12) uniform highp sampler2D g_Texture_GBufferDepth;
layout(binding = 17) uniform highp sampler2D g_Texture_RadianceDirect;

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
	highp vec4 g_FrustumPlane[6];
};

layout(row_major, binding = 7) uniform g_BufferConstant_Light
{
	highp mat4x4 g_LightView;
	highp mat4x4 g_LightProjection;
	highp vec4 g_LightIrradiance;
	highp vec4 g_LightDirection;
};

layout(location = 0) out highp vec4 OM_RadianceTotal;

layout (early_fragment_tests) in;

void main() 
{
	highp vec3 NormalMap_LowFreq = texture(g_NormalMap_LowFreq, RS_UV_NormalMap_LowFreq).xyz*2.0f - 1.0f;
	highp vec3 NormalMap_HighFreq = texture(g_NormalMap_HighFreq, RS_UV_NormalMap_HighFreq).xyz*2.0f - 1.0f;
	highp vec3 NormalTangent = normalize(NormalMap_LowFreq + NormalMap_HighFreq);

	highp vec3 Axis_Tangent = normalize(RS_Tangent);
	highp vec3 Axis_Bitangent = normalize(RS_Bitangent);
	highp vec3 Axis_Normal = cross(Axis_Bitangent, Axis_Tangent);

	//Material
	highp vec3 NormalMacro = Axis_Normal;
	highp vec3 NormalMeso = mat3x3(Axis_Tangent.xyz, Axis_Bitangent.xyz, Axis_Normal.xyz)*NormalTangent;
	highp vec3 ColorAlbedo = vec3(0.5f,0.5f,0.5f);
    highp vec3 ColorSpecular = vec3(0.0f,0.5f,0.5f);
	highp float Glossiness = 1.0f;

	//Forward
	//Light_Directional
	
	highp vec3 ViewRadiance = vec3(0.0f, 0.0f, 0.0f);

	highp vec3 ViewDirection = normalize(g_InvView[3].xyz - RS_PositionWorld); //PositionSensor_World - Position_World

	highp vec3 LightDirection = - normalize(g_LightDirection.xyz);

	//E_N
    highp vec3 IrradianceNormal = g_LightIrradiance.xyz*max(0.0f, dot(NormalMeso, LightDirection));

    //Normalized Blinn-Phong
    highp vec3 HalfVector = ViewDirection + LightDirection; //即NormalMicro

    highp float Smoothness = Glossiness * 5.0f; //与Glossiness正相关 

    highp float NormalDistribution =  ((Smoothness + 2.0f)/6.28f) * pow(max(0.0f, dot(HalfVector,NormalMeso)),Smoothness);

    highp vec3 Fresnel = ColorSpecular;

    highp vec3 BRDF_Specular = NormalDistribution * Fresnel * 0.25f; //G(l,v) = max(0,cos<LightDirection,NormalMeso>)*max(0,cos<ViewDirection,NormalMeso>)

    //Lambert
    highp vec3 ColorDiffuse = ColorAlbedo * (vec3(1.0f,1.0f,1.0f) - Fresnel);

    highp vec3 BRDF_Diffuse = ColorDiffuse / 3.14f;

    //L_V
    ViewRadiance += (BRDF_Specular + BRDF_Diffuse) * IrradianceNormal;    

	//Forward
	//Light_ScreenSpaceReflection
	do{
		highp vec3 Position_World = RS_PositionWorld;
		highp vec3 ReflectionDirection = NormalMacro * dot(ViewDirection,NormalMacro)*2.0f - ViewDirection;

		//RayTrace
    	highp vec3 Position_RayFrom_FrameBuffer = vec3(gl_FragCoord.xyz);

		highp vec3 Position_RayTo_FrameBuffer;
    	{
        	highp vec3 Position_RayTo_World = Position_World + ReflectionDirection;

        	highp vec4 Position_RayTo_View = g_View * vec4(Position_RayTo_World, 1.0f);
                       
        	//Clip to NearZ 
        	if(Position_RayTo_View.z > -0.1f) //右手系
			{
				break; //Alpha Blend ADD
			}

        	highp vec4 Position_RayTo_NormalizedDevice = g_Projection * Position_RayTo_View;
        	Position_RayTo_NormalizedDevice.xyzw /= Position_RayTo_NormalizedDevice.wwww;
            
        	Position_RayTo_FrameBuffer = vec3(((Position_RayTo_NormalizedDevice.x+1.0f)*0.5f)*800.0f, ((Position_RayTo_NormalizedDevice.y+1.0f)*0.5f)*600.0f, Position_RayTo_NormalizedDevice.z);//Viewport
    	}

		//Permute Axis
    	bool bLongX = abs(Position_RayTo_FrameBuffer.x - Position_RayFrom_FrameBuffer.x) > abs(Position_RayTo_FrameBuffer.y - Position_RayFrom_FrameBuffer.y);
    	highp float LongFrom = bLongX ? Position_RayFrom_FrameBuffer.x : Position_RayFrom_FrameBuffer.y;
    	highp float LongTo = bLongX ? Position_RayTo_FrameBuffer.x : Position_RayTo_FrameBuffer.y;
    	highp float ShortFrom = bLongX ? Position_RayFrom_FrameBuffer.y : Position_RayFrom_FrameBuffer.x;
    	highp float ShortTo = bLongX ? Position_RayTo_FrameBuffer.y : Position_RayTo_FrameBuffer.x;

		highp float DeltaLong;
    	highp float DeltaShort;
    	highp float DeltaDepth;
		{
        	DeltaLong = sign(LongTo - LongFrom);
        	//Line Degenerate
	    	highp float AbsLong = abs(LongTo - LongFrom);
        	if(AbsLong <= 1.0f)
        	{
            	break; //Alpha Blend ADD
        	}
        	highp float InverseAbsLong = 1.0f / AbsLong;
        	DeltaShort = (ShortTo - ShortFrom) * InverseAbsLong;
	    	DeltaDepth = (Position_RayTo_FrameBuffer.z - Position_RayFrom_FrameBuffer.z) * InverseAbsLong; //不需要透视校正插值
		}
		//Stride
		DeltaLong*=4.0f;
		DeltaShort*=4.0f;
		DeltaDepth*=4.0f;
	
    	//1/2 Pixel
    	highp float LongCurrent = LongFrom + DeltaLong*0.5f;
    	highp float ShortCurrent = ShortFrom + DeltaShort*0.5f;
    	highp float DepthCurrent = Position_RayFrom_FrameBuffer.z + DeltaDepth*0.5f;
    	highp vec2 TexelCoord = bLongX ? vec2(LongCurrent, ShortCurrent) : vec2(ShortCurrent, LongCurrent);

		highp vec3 LightRadiance = vec3(0.0f,0.0f,0.0f);

    	for(int i_Step = 0; i_Step < 100; ++i_Step)
    	{
        	LongCurrent += DeltaLong;
        	ShortCurrent += DeltaShort;
        	DepthCurrent += DeltaDepth;
        	TexelCoord = bLongX ? vec2(LongCurrent, ShortCurrent) : vec2(ShortCurrent, LongCurrent);

        	if(TexelCoord.x <= 0.0f || TexelCoord.x >= 799.0f || TexelCoord.y <= 0.0f || TexelCoord.y >= 599.0f)
        	{
            	//LightRadiance = vec3(0.0f,0.0f,0.0f);
            	//LightRadiance = texelFetch(g_Texture_RadianceDirect, ivec2(TexelCoord.xy), 0).xyz;
				break;
        	}

			highp float DepthScene = texelFetch(g_Texture_GBufferDepth, ivec2(TexelCoord.xy), 0).x;

        	if(DepthCurrent < DepthScene) //&& DepthScene > 0.00001f) //在SkyBox之前绘制即可 //Reverse-Z //RayCastTerrain 
        	{
				LightRadiance = texelFetch(g_Texture_RadianceDirect, ivec2(TexelCoord.xy), 0).xyz;
            	break;
        	}
    	}

		//Environment Map
    	highp vec3 Fresnel = ColorSpecular;
    	ViewRadiance +=  Fresnel*LightRadiance;
	}while(false);

	//Forward
	//Light_Refraction
	{
		highp vec3 LightRadiance = texelFetch(g_Texture_RadianceDirect, ivec2(gl_FragCoord.xy+NormalMeso.xy*vec2(800.0f,600.0f)*0.05f), 0).xyz;//vScale:0.05f
		highp vec3 Transmittance = ColorAlbedo;
		ViewRadiance += Transmittance*LightRadiance;
	}

	OM_RadianceTotal = vec4(ViewRadiance,1.0f);
}