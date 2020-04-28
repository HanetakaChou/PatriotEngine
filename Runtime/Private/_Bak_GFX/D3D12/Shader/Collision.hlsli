struct Frustum
{
	float4 Orientation;	// Quaternion representing rotation.
	float3 Origin;	// Origin of the frustum.
	float RightSlope;	// Positive X slope (X/Z).
	float LeftSlope;	// Negative X slope.
	float TopSlope;	// Positive Y slope (Y/Z).
	float BottomSlope;	// Negative Y slope.
	float Near;	// Z of the near plane
	float Far;	// Z of the far plane
};

struct Sphere
{
	float3 Center;	// Center of the sphere.
	float Radius;	// Radius of the sphere.
};