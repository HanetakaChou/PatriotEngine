Shader "Hidden/SSAO_CacheEfficient/BoxBlur4x4"
{
	Properties
	{
		_MainTex ("Texture", 2D) = "white" {}
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
			
			sampler2D _MainTex;

			float4 frag (v2f i) : SV_Target0
			{
				const float4 vec4LeftTop = tex2D(_MainTex, i.uv + float2(-0.5f, -0.5f) / _ScreenParams.xy);
				const float4 vec4LeftBottom = tex2D(_MainTex, i.uv + float2(-0.5f, 1.5f) / _ScreenParams.xy);
				const float4 vec4RightTop = tex2D(_MainTex, i.uv + float2(1.5f, -0.5f) / _ScreenParams.xy);
				const float4 vec4RightBottom = tex2D(_MainTex, i.uv + float2(1.5f, 1.5f) / _ScreenParams.xy);
				return (vec4LeftTop + vec4LeftBottom + vec4RightTop + vec4RightBottom)*0.25f;
			}
			ENDCG
		}
	}
}
