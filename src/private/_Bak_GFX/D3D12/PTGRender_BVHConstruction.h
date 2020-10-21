

#include <d3d12.h>

//BVH(Bounding Volume Hierarchy) Construction

//Christian Lauterbach, Michael Garland, Shubhabrata Sengupta, David Luebke, Dinesh Manocha. "Fast BVH Construction on GPUs." Eurographics 2009.
//http://research.nvidia.com/publication/fast-bvh-construction-gpus

//Tero Karras. "Maximizing Parallelism in the Construction of BVHs, Octrees, and k-d Trees." HPC 2012.
//http://research.nvidia.com/publication/maximizing-parallelism-construction-bvhs-octrees-and-k-d-trees

//Tero Karras, Timo Aila. "Fast Parallel Construction of High-Quality Bounding Volume Hierarchies." HPC 2013.
//http://research.nvidia.com/publication/fast-parallel-construction-high-quality-bounding-volume-hierarchies

class PTGRender_BVHConstruction
{
public:
	virtual void Execute(
		ID3D12Device *pDeviceCompute
	) = 0;
};

class PTGRender_BVHConstruction_GPU :public PTGRender_BVHConstruction
{
	inline PTGRender_BVHConstruction_GPU();
	
};