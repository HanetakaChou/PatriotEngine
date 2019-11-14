#version 310 es
layout(location = 7) in highp vec2 l_Screen_UV;

layout(row_major, binding = 3) uniform g_BufferConstant_Sensor
{
	highp mat4x4 g_View;
	highp mat4x4 g_Projection;
	highp mat4x4 g_InvView;
	highp mat4x4 g_InvProjection;
};

layout(binding = 12) uniform highp sampler2D g_Texture_GBufferDepth;
layout(binding = 13) uniform highp sampler2D g_Texture_GBufferNormalMacro;
//layout(binding = 14) uniform highp sampler2D g_Texture_GBufferNormalMeso; 使用NormalMap后 SSR将无法得到正确的效果
layout(binding = 15) uniform highp sampler2D g_Texture_GBufferAlbedo;
layout(binding = 16) uniform highp sampler2D g_Texture_GBufferSpecular;
layout(binding = 17) uniform highp sampler2D g_Texture_RadianceDirect;

layout(location = 0) out highp vec4 SV_Target0;

layout (early_fragment_tests) in;

void main() 
{
    //GBuffer
    highp vec3 GBuffer_Specular = texelFetch(g_Texture_GBufferSpecular, ivec2(gl_FragCoord.xy), 0).xyz;
    highp vec3 GBuffer_NormalMacro = (texelFetch(g_Texture_GBufferNormalMacro, ivec2(gl_FragCoord.xy), 0).xyz)*2.0f-1.0f;
    highp float GBuffer_Depth = texelFetch(g_Texture_GBufferDepth, ivec2(gl_FragCoord.xy), 0).x;

	//SpecularFilter
    if(dot(GBuffer_Specular, vec3(0.2126f,0.7152f,0.0722f)) < 0.2f)
    {
        discard; //Alpha Blend ADD
    }

    //ReflectionDirection
    highp vec3 Position_World;
    {
        highp vec4 Position_NormalizedDevice;
        Position_NormalizedDevice.x = l_Screen_UV.x*2.0f - 1.0f;
        Position_NormalizedDevice.y = l_Screen_UV.y*2.0f - 1.0f;
        Position_NormalizedDevice.z = GBuffer_Depth;
        Position_NormalizedDevice.w = 1.0f;
            
        highp vec4 Position_View = g_InvProjection * Position_NormalizedDevice;
        Position_View.xyzw /= Position_View.wwww;

        Position_World = (g_InvView * Position_View).xyz;
    }
	
    highp vec3 ViewDirection;
    {
        highp vec3 PositionSensor_World = g_InvView[3].xyz; //g_InvView * vec4(0.0f,0.0f,0.0f,1.0f);

        ViewDirection = normalize(PositionSensor_World - Position_World);
    }

	highp vec3 NormalMacro = GBuffer_NormalMacro;
		
    highp vec3 ReflectionDirection = NormalMacro * dot(ViewDirection,NormalMacro)*2.0f - ViewDirection;

    //RayTrace
    highp vec3 Position_RayFrom_FrameBuffer = vec3(gl_FragCoord.xy, GBuffer_Depth);

    highp vec3 Position_RayTo_FrameBuffer;
    {
        highp vec3 Position_RayTo_World = Position_World + ReflectionDirection;

        highp vec4 Position_RayTo_View = g_View * vec4(Position_RayTo_World, 1.0f);
                       
        //Clip to NearZ 
        if(Position_RayTo_View.z > -0.1f) //右手系
		{
			discard; //Alpha Blend ADD
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
            discard; //Alpha Blend ADD
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
    highp vec3 Fresnel = GBuffer_Specular;
    highp vec3 RadianceIndirect = Fresnel*LightRadiance;
	SV_Target0 = vec4(RadianceIndirect,1.0f);
}