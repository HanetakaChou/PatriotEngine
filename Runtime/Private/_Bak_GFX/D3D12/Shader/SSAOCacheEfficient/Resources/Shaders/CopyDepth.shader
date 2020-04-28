Shader "Hidden/SSAO_CacheEfficient/CopyDepth"
{
	Properties
	{

	}
	SubShader
	{
		// No culling or depth
		Cull Off ZWrite Off ZTest Always

		Pass
		{
			CGPROGRAM
			#pragma vertex vert
			#pragma fragment frag
			//#pragma enable_d3d11_debug_symbols

			#include "UnityCG.cginc"

			struct appdata
			{
				float4 vertex : POSITION;
				float2 uv : TEXCOORD0;
			};

			struct v2f
			{
				float2 uv : TEXCOORD0;
				float4 vertex : SV_POSITION;
			};

			v2f vert (appdata v)
			{
				v2f o;
				o.vertex = UnityObjectToClipPos(v.vertex);
				o.uv = v.uv;
				return o;
			}

			// System built-in variables
			//sampler2D _CameraGBufferTexture2;
			UNITY_DECLARE_DEPTH_TEXTURE(_CameraDepthTexture);
			//sampler2D _CameraDepthNormalsTexture;

			float frag (v2f i):SV_TARGET0
			{
				return SAMPLE_DEPTH_TEXTURE(_CameraDepthTexture, i.uv);
			}
			ENDCG
		}
	}
}
