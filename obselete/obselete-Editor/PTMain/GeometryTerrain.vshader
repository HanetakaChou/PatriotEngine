cbuffer g_CBuffer_Transform:register(b0)
{
	column_major float4x4 g_TransformModel;
	column_major float4x4 g_TransformView;
	column_major float4x4 g_TransformProjection;
};

struct Vertex_VS_IN
{
	float3 PositionModelSpace:Position0;
};

struct Vertex_VS_Out
{
	float4 gl_Position:SV_Position;
};

Vertex_VS_Out main(Vertex_VS_IN vertex)
{
	Vertex_VS_Out gl_PerVertex;
    gl_PerVertex.gl_Position = mul(mul(mul(float4(vertex.PositionModelSpace, 1.0f), g_TransformModel), g_TransformView), g_TransformProjection);
	return gl_PerVertex;
}