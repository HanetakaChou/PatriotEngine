private:
	struct GrassData
	{
		PTVector4F m_Billborad_Origin;
		PTVector2F m_Wave_Translation_InitialPhase;
		PTVector4F m_Texture_Index;
	};

	//由于四叉树中所有叶节点的深度都相同
	//因此并不需要存储中间节点和树状结构，可以根据长方形的位置，推算出相应的节点在树中的位置
	struct Patch
	{
		std::vector<GrassData> m_Grass;
	} m_Patch[16][16];//Tile:16×16*(Patch:16×16) //与Terrain一致 

	void CullAndAnimate_Helper(
		int XMax, int XMin, int ZMax, int ZMin,//Iterate
		PTFrustumSIMD &Frustum,//Cull
		float fTime,//Animate
		std::vector<PTVector4F> &Billborad_Origin, std::vector<PTVector4F> &Wave_Translation, std::vector<PTVector4F> &Texture_Index
	);
public:
	void CullAndAnimate(PTMatrixSIMD ViewProjection, float fTime, std::vector<PTVector4F> &Billborad_Origin, std::vector<PTVector4F> &Wave_Translation, std::vector<PTVector4F> &Texture_Index);



void Scene_Geometry_Grass::CullAndAnimate_Helper(
	int XMax, int XMin, int ZMax, int ZMin,//Iterate
	PTFrustumSIMD &Frustum,//Cull
	float fTime,//Animate
	std::vector<PTVector4F> &Billborad_Origin, std::vector<PTVector4F> &Wave_Translation, std::vector<PTVector4F> &Texture_Index
)
{
	PTVector3F AAB_Min{ XMin ,0.0f,ZMin };
	PTVector3F AAB_Max{ XMax ,2000.0f,ZMax};//Height自行确定
	PTAABSIMD AAB = ::PTAABSIMDLoad(&AAB_Min, &AAB_Max);

	PTIntersectionType IntersectionType =::PTFrustumSIMDAABSIMDIntersect(Frustum, AAB);

	if (IntersectionType == PTDISJOINT)
	{
		//Early Out
		//All Child Node Disjoint
		//return;
	}
	else
	{
		assert((XMax - XMin) == (ZMax - ZMin));
		assert(((XMax - XMin)&(XMax - XMin - 1)) == 0);
		assert((XMax - XMin) >= 16);

		if ((XMax - XMin) == 16)//Leaf Node
		{
			//叶节点
			//递归回溯

			int i_x = (XMin + 128) / 16;
			int i_y = (ZMin + 128) / 16;

			for (GrassData &rGrassData : m_Patch[i_x][i_y].m_Grass)
			{
				Billborad_Origin.push_back(rGrassData.m_Billborad_Origin);
				//Animate
				Wave_Translation.push_back(
					PTVector4F{
					0.2f*::sinf(fTime + rGrassData.m_Wave_Translation_InitialPhase.x),//振幅 Amplitude = 0.2f
					0.0f,
					0.2f*::sinf(fTime + rGrassData.m_Wave_Translation_InitialPhase.y),//振幅 Amplitude = 0.2f
					1.0f });
				Texture_Index.push_back(rGrassData.m_Texture_Index);
			}

			//return;
		}
		else//Internal Node
		{
			if (IntersectionType == PTCONTAIN)
			{
				//Early Out
				//All Child Node Contain

				int i_x_min = (XMin + 128) / 16;
				int i_x_max = (XMax + 128) / 16 - 1;
				int i_y_min = (ZMin + 128) / 16;
				int i_y_max = (ZMax + 128) / 16 - 1;

				assert(i_x_min >= 0);
				assert(i_x_max <= 15);
				assert(i_y_min >= 0);
				assert(i_y_max <= 15);

				for (int i_x = i_x_min; i_x <= i_x_max; ++i_x)
				{
					for (int i_y = i_y_min; i_y <= i_y_max; ++i_y)
					{
						for (GrassData &rGrassData : m_Patch[i_x][i_y].m_Grass)
						{
							Billborad_Origin.push_back(rGrassData.m_Billborad_Origin);
							//Animate
							Wave_Translation.push_back(
								PTVector4F{
								0.2f*::sinf(fTime + rGrassData.m_Wave_Translation_InitialPhase.x),//振幅 Amplitude = 0.2f
								0.0f,
								0.2f*::sinf(fTime + rGrassData.m_Wave_Translation_InitialPhase.y),//振幅 Amplitude = 0.2f
								1.0f });
							Texture_Index.push_back(rGrassData.m_Texture_Index);
						}
					}
				}

				//return;

			}
			else if (IntersectionType == PTINTERSECT)
			{

				//递归深入
				//Four Child Node

				this->CullAndAnimate_Helper(
					XMax, (XMax + XMin) / 2, ZMax, (ZMax + ZMin) / 2,
					Frustum,
					fTime,
					Billborad_Origin, Wave_Translation, Texture_Index
				);

				this->CullAndAnimate_Helper(
					XMax, (XMax + XMin) / 2, (ZMax + ZMin) / 2, ZMin,
					Frustum,
					fTime,
					Billborad_Origin, Wave_Translation, Texture_Index
				);

				this->CullAndAnimate_Helper(
					(XMax + XMin) / 2, XMin, ZMax, (ZMax + ZMin) / 2,
					Frustum,
					fTime,
					Billborad_Origin, Wave_Translation, Texture_Index
				);

				this->CullAndAnimate_Helper(
					(XMax + XMin) / 2, XMin, (ZMax + ZMin) / 2, ZMin,
					Frustum,
					fTime,
					Billborad_Origin, Wave_Translation, Texture_Index
				);
			}
		}
	}	
}

void Scene_Geometry_Grass::CullAndAnimate(PTMatrixSIMD ViewProjection, float fTime, std::vector<PTVector4F> &Billborad_Origin, std::vector<PTVector4F> &Wave_Translation, std::vector<PTVector4F> &Texture_Index)
{
	PTFrustumSIMD Frustum = ::PTFrustumSIMDLoadRH(ViewProjection);
	this->CullAndAnimate_Helper(
		128, -128, 128, -128,
		Frustum,
		fTime,
		Billborad_Origin, Wave_Translation, Texture_Index
	);
}