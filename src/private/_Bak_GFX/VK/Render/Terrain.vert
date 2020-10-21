#version 310 es

layout(location = 0) in vec2 IA_Patch_PositionTangentXY;
layout(location = 1) in vec2 IA_Patch_UV;

layout(location = 6) out vec2 HS_Patch_PositionTangentXY;
layout(location = 7) out vec2 HS_Patch_UV;

void main() 
{
	HS_Patch_PositionTangentXY = IA_Patch_PositionTangentXY;
	HS_Patch_UV = IA_Patch_UV;
}