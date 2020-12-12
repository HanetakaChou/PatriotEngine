Shader "Hidden/SSAO_CacheEfficient/PresentAOInverse"
{
	Properties
	{
		_MainTex ("Texture", 2D) = "white" {}
		_TexAOInverse ("Texture", 2D) = "white" {}
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
			sampler2D _TexAOInverse;

			float4 frag (v2f i) : SV_Target0
			{
				float4 Radiance = tex2D(_MainTex, i.uv);
				float3 Ambient = tex2D(_TexAOInverse, i.uv).xxx * float3(0.1f,0.1f,0.1f); //*ColorDiffuse In GBuffer
				return float4(Radiance.xyz + Ambient, Radiance.w);
				//return float4(Ambient ,1.0f);
			}
			ENDCG
		}
	}
}
