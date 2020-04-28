using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Rendering;

[AddComponentMenu("Rendering/SSAO_CacheEfficient", -1)]
[ExecuteInEditMode]
[ImageEffectAllowedInSceneView]
[DisallowMultipleComponent]
[RequireComponent(typeof(Camera))]
public class SSAO_CacheEfficient : MonoBehaviour {

	// Use this for initialization
	void Start () {

	}

	//ComputeShader m_pCS_DeinterleaveDepth;

	Material m_pPS_CopyDepth;

	RenderTexture m_pTextureDepth;

	ComputeShader m_pCS_DeinterleaveDepth;

	RenderTexture m_pTextureDepthDeinterleaved;

	ComputeShader m_pCS_AOInverse;

	RenderTexture m_pTextureAOInverseDeinterleaved;

	ComputeShader m_pCS_ReinterleaveAOInverse;

	RenderTexture m_pTextureAOInverse;

	Material m_pPS_BoxBlur4x4;

	RenderTexture m_pTextureAOInverseBlurred;

	Material m_pPS_PresentAOInverse;

	CommandBuffer m_pCommandBufferSSAO;

	void OnEnable()
	{
		Shader pShaderTemp = Shader.Find("Hidden/SSAO_CacheEfficient/CopyDepth");
		UnityEngine.Assertions.Assert.IsNotNull (pShaderTemp);
		m_pPS_CopyDepth = new Material(pShaderTemp);
		UnityEngine.Assertions.Assert.IsNotNull (m_pPS_CopyDepth);

		m_pCS_DeinterleaveDepth = Resources.Load<ComputeShader>("Shaders/DeinterleaveDepth");
		UnityEngine.Assertions.Assert.IsNotNull (m_pCS_DeinterleaveDepth);

		m_pCS_AOInverse = Resources.Load<ComputeShader>("Shaders/AOInverse");
		UnityEngine.Assertions.Assert.IsNotNull (m_pCS_AOInverse);

		m_pCS_ReinterleaveAOInverse = Resources.Load<ComputeShader>("Shaders/ReinterleaveAOInverse");
		UnityEngine.Assertions.Assert.IsNotNull (m_pCS_ReinterleaveAOInverse);

		pShaderTemp = Shader.Find ("Hidden/SSAO_CacheEfficient/BoxBlur4x4");
		UnityEngine.Assertions.Assert.IsNotNull (pShaderTemp);
		m_pPS_BoxBlur4x4 = new Material(pShaderTemp);
		UnityEngine.Assertions.Assert.IsNotNull (m_pPS_BoxBlur4x4);

		pShaderTemp = Shader.Find ("Hidden/SSAO_CacheEfficient/PresentAOInverse");
		UnityEngine.Assertions.Assert.IsNotNull (pShaderTemp);
		m_pPS_PresentAOInverse = new Material(pShaderTemp);
		UnityEngine.Assertions.Assert.IsNotNull (m_pPS_PresentAOInverse);

		Camera pCamera = GetComponent<Camera> ();

		m_pTextureDepth = new RenderTexture (pCamera.pixelWidth , pCamera.pixelHeight , 0, RenderTextureFormat.RHalf, RenderTextureReadWrite.Linear);
		m_pTextureDepth.volumeDepth = 1;
		m_pTextureDepth.dimension = TextureDimension.Tex2D;
		m_pTextureDepth.enableRandomWrite = true;
		bool bResult = m_pTextureDepth.Create ();
		UnityEngine.Assertions.Assert.IsTrue (bResult);

		m_pTextureDepthDeinterleaved = new RenderTexture (pCamera.pixelWidth / 4, pCamera.pixelHeight / 4, 0, RenderTextureFormat.RHalf, RenderTextureReadWrite.Linear);
		m_pTextureDepthDeinterleaved.volumeDepth = 16;
		m_pTextureDepthDeinterleaved.dimension = TextureDimension.Tex2DArray;
		m_pTextureDepthDeinterleaved.enableRandomWrite = true;
		bResult = m_pTextureDepthDeinterleaved.Create ();
		UnityEngine.Assertions.Assert.IsTrue (bResult);

		m_pTextureAOInverseDeinterleaved = new RenderTexture (pCamera.pixelWidth / 4, pCamera.pixelHeight / 4, 0, RenderTextureFormat.RHalf, RenderTextureReadWrite.Linear);
		m_pTextureAOInverseDeinterleaved.volumeDepth = 16;
		m_pTextureAOInverseDeinterleaved.dimension = TextureDimension.Tex2DArray;
		m_pTextureAOInverseDeinterleaved.enableRandomWrite = true;
		bResult = m_pTextureAOInverseDeinterleaved.Create ();
		UnityEngine.Assertions.Assert.IsTrue (bResult);

		m_pTextureAOInverse = new RenderTexture (pCamera.pixelWidth , pCamera.pixelHeight , 0, RenderTextureFormat.RHalf, RenderTextureReadWrite.Linear);
		m_pTextureAOInverse.volumeDepth = 1;
		m_pTextureAOInverse.dimension = TextureDimension.Tex2D;
		m_pTextureAOInverse.enableRandomWrite = true;
		bResult = m_pTextureAOInverse.Create ();
		UnityEngine.Assertions.Assert.IsTrue (bResult);

		m_pTextureAOInverseBlurred = new RenderTexture (pCamera.pixelWidth , pCamera.pixelHeight , 0, RenderTextureFormat.RHalf, RenderTextureReadWrite.Linear);
		m_pTextureAOInverseBlurred.volumeDepth = 1;
		m_pTextureAOInverseBlurred.dimension = TextureDimension.Tex2D;
		m_pTextureAOInverseBlurred.enableRandomWrite = true;
		bResult = m_pTextureAOInverseBlurred.Create ();
		UnityEngine.Assertions.Assert.IsTrue (bResult);

		//
		m_pCommandBufferSSAO = new CommandBuffer();
		m_pCommandBufferSSAO.name = "SSAO";
		pCamera.AddCommandBuffer (CameraEvent.BeforeImageEffectsOpaque, m_pCommandBufferSSAO);
	}

	void OnPreCull()
	{

	}

	void OnPreRender()
	{
		Camera pCamera = GetComponent<Camera> ();
		pCamera.depthTextureMode = DepthTextureMode.Depth;

		m_pCommandBufferSSAO.Clear ();

		//CopyDepth //Unity3D Can Not Bind Depth On Compute Shader
		m_pCommandBufferSSAO.Blit (null, new RenderTargetIdentifier (m_pTextureDepth), m_pPS_CopyDepth, 0);

		//DeinterleaveDepth
		m_pCommandBufferSSAO.SetComputeTextureParam (
			m_pCS_DeinterleaveDepth,
			m_pCS_DeinterleaveDepth.FindKernel ("main"),
			"g_TextureDepth",
			new RenderTargetIdentifier (m_pTextureDepth)
		);
		m_pCommandBufferSSAO.SetComputeTextureParam (
			m_pCS_DeinterleaveDepth,
			m_pCS_DeinterleaveDepth.FindKernel ("main"),
			"g_TextureDepthDeinterleaved",
			new RenderTargetIdentifier (m_pTextureDepthDeinterleaved)
		);
		m_pCommandBufferSSAO.DispatchCompute (
			m_pCS_DeinterleaveDepth,
			m_pCS_DeinterleaveDepth.FindKernel ("main"),
			(pCamera.pixelWidth - 1) / 16 + 1,
			(pCamera.pixelHeight - 1) / 16 + 1,
			1
		);

		//AOInverse
		m_pCommandBufferSSAO.SetComputeVectorParam (
			m_pCS_AOInverse,
			"g_vec2FrameBufferSize",
			new Vector4 (pCamera.pixelWidth, pCamera.pixelHeight, 0.0f, 0.0f)
		);
		m_pCommandBufferSSAO.SetComputeTextureParam (
			m_pCS_AOInverse,
			m_pCS_AOInverse.FindKernel ("main"),
			"g_TextureDepthDeinterleaved",
			new RenderTargetIdentifier (m_pTextureDepthDeinterleaved)
		);
		m_pCommandBufferSSAO.SetComputeTextureParam (
			m_pCS_AOInverse,
			m_pCS_AOInverse.FindKernel ("main"),
			"g_TextureAOInverseDeinterleaved",
			new RenderTargetIdentifier (m_pTextureAOInverseDeinterleaved)
		);
		m_pCommandBufferSSAO.DispatchCompute (
			m_pCS_AOInverse,
			m_pCS_AOInverse.FindKernel ("main"),
			(pCamera.pixelWidth - 1) / 64 + 1,
			(pCamera.pixelHeight - 1) / 64 + 1,
			16
		);

		//ReinterleaveAOInverse
		m_pCommandBufferSSAO.SetComputeTextureParam (
			m_pCS_ReinterleaveAOInverse,
			m_pCS_ReinterleaveAOInverse.FindKernel ("main"),
			"g_TextureAOInverseDeinterleaved",
			new RenderTargetIdentifier (m_pTextureAOInverseDeinterleaved)
		);

		m_pCommandBufferSSAO.SetComputeTextureParam (
			m_pCS_ReinterleaveAOInverse,
			m_pCS_ReinterleaveAOInverse.FindKernel ("main"),
			"g_TextureAOInverse",
			new RenderTargetIdentifier (m_pTextureAOInverse)
		);

		m_pCommandBufferSSAO.DispatchCompute (
			m_pCS_ReinterleaveAOInverse,
			m_pCS_ReinterleaveAOInverse.FindKernel ("main"),
			(pCamera.pixelWidth - 1) / 64 + 1,
			(pCamera.pixelHeight - 1) / 64 + 1,
			16
		);

		//Blur
		m_pCommandBufferSSAO.Blit (
			m_pTextureAOInverse,
			new RenderTargetIdentifier (m_pTextureAOInverseBlurred),
			m_pPS_BoxBlur4x4,
			0
		);
	}

	void OnRenderImage(RenderTexture source, RenderTexture destination)
	{
		//PresentAOInverse
		m_pPS_PresentAOInverse.SetTexture("_TexAOInverse",m_pTextureAOInverseBlurred);
		Graphics.Blit (source, destination, m_pPS_PresentAOInverse, 0);
	}

	// Update is called once per frame
	void Update () {

	}

	void OnDisable()
	{
		Camera pCamera = GetComponent<Camera> ();

		pCamera.RemoveCommandBuffer(CameraEvent.BeforeImageEffectsOpaque, m_pCommandBufferSSAO);
	}
}
