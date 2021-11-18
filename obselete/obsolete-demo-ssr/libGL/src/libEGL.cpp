#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN

#define EGLAPI
#include "../include/EGL/egl.h"
#define EGL_EGLEXT_PROTOTYPES
#include "../include/EGL/eglext.h"
#define GL_APICALL
#include "../include/GLES3/gl31.h"
#define GL_GLEXT_PROTOTYPES
#include "../include/GLES3/gl2ext.h"

#include "../include/wglext.h"

#include <assert.h>
#include <vector>
#include <map>
#include <algorithm>

//#define ESPROFILE 1

static EGLint s_EGLError = EGL_SUCCESS;
extern "C" EGLint EGLAPIENTRY eglGetError(void)
{
	return s_EGLError;
}

extern "C" EGLDisplay EGLAPIENTRY eglGetDisplay(EGLNativeDisplayType display_id)
{
	return static_cast<HDC>(display_id);
}

static PFNWGLCREATECONTEXTATTRIBSARBPROC s_pFn_wglCreateContextAttribsARB = NULL;
static PFNWGLMAKECONTEXTCURRENTARBPROC s_pFn_wglMakeContextCurrentARB = NULL;
static PFNWGLSWAPINTERVALEXTPROC s_pFn_wglSwapIntervalEXT = NULL;

static PFNGLDEBUGMESSAGECONTROLKHRPROC s_pFn_glDebugMessageControlKHR = NULL;
static PFNGLDEBUGMESSAGECALLBACKKHRPROC s_pFn_glDebugMessageCallbackKHR = NULL;

static PFNGLGENBUFFERSPROC s_pFn_glGenBuffers = NULL;
static PFNGLGENTEXTURESPROC s_pFn_glGenTextures = NULL;

static PFNGLBINDBUFFERPROC s_pFn_glBindBuffer = NULL;
static PFNGLACTIVETEXTUREPROC s_pFn_glActiveTexture = NULL;
static PFNGLBINDTEXTUREPROC s_pFn_glBindTexture = NULL;

static PFNGLBUFFERDATAPROC s_pFn_glBufferData = NULL;
static PFNGLTEXBUFFEROESPROC s_pFn_glTexBufferOES = NULL;
static PFNGLTEXSTORAGE2DPROC s_pFn_glTexStorage2D = NULL;
static PFNGLTEXSTORAGE3DPROC s_pFn_glTexStorage3D = NULL;
static PFNGLTEXSTORAGE2DMULTISAMPLEPROC s_pFn_glTexStorage2DMultisample = NULL;
static PFNGLCOMPRESSEDTEXIMAGE3DOESPROC s_pFn_glCompressedTexImage3DOES = NULL;

static PFNGLBUFFERSUBDATAPROC s_pFn_glBufferSubData = NULL;
static PFNGLTEXSUBIMAGE2DPROC s_pFn_glTexSubImage2D = NULL;
static PFNGLTEXSUBIMAGE3DPROC s_pFn_glTexSubImage3D = NULL;

static PFNGLGENSAMPLERSPROC s_pFn_glGenSamplers = NULL;
static PFNGLSAMPLERPARAMETERIPROC s_pFn_glSamplerParameteri = NULL;
static PFNGLSAMPLERPARAMETERFVPROC s_pFn_glSamplerParameterfv = NULL;

static PFNGLGENFRAMEBUFFERSPROC s_pFn_glGenFramebuffers = NULL;
static PFNGLBINDFRAMEBUFFERPROC s_pFn_glBindFramebuffer = NULL;
static PFNGLFRAMEBUFFERTEXTURE2DPROC s_pFn_glFramebufferTexture2D = NULL;
static PFNGLREADBUFFERPROC s_pFn_glReadBuffer = NULL;
static PFNGLDRAWBUFFERSPROC s_pFn_glDrawBuffers = NULL;
static PFNGLCHECKFRAMEBUFFERSTATUSPROC s_pFn_glCheckFramebufferStatus = NULL;

static PFNGLCREATESHADERPROC s_pFn_glCreateShader = NULL;
static PFNGLSHADERSOURCEPROC s_pFn_glShaderSource = NULL;
static PFNGLCOMPILESHADERPROC s_pFn_glCompileShader = NULL;
static PFNGLGETSHADERIVPROC s_pFn_glGetShaderiv = NULL;
static PFNGLGETSHADERINFOLOGPROC s_pFn_glGetShaderInfoLog = NULL;

static PFNGLCREATEPROGRAMPROC s_pFn_glCreateProgram = NULL;
static PFNGLATTACHSHADERPROC s_pFn_glAttachShader = NULL;
static PFNGLLINKPROGRAMPROC s_pFn_glLinkProgram = NULL;
static PFNGLGETPROGRAMIVPROC s_pFn_glGetProgramiv = NULL;
static PFNGLGETPROGRAMINFOLOGPROC s_pFn_glGetProgramInfoLog = NULL;

static PFNGLGENVERTEXARRAYSPROC s_pFn_glGenVertexArrays = NULL;
static PFNGLBINDVERTEXARRAYPROC s_pFn_glBindVertexArray = NULL;
static PFNGLVERTEXATTRIBFORMATPROC s_pFn_glVertexAttribFormat = NULL;
static PFNGLENABLEVERTEXATTRIBARRAYPROC s_pFn_glEnableVertexAttribArray = NULL;
static PFNGLVERTEXATTRIBBINDINGPROC s_pFn_glVertexAttribBinding = NULL;

//General
static PFNGLENABLEPROC s_pFn_glEnable = NULL;
static PFNGLDISABLEPROC s_pFn_glDisable = NULL;
//OM / Target
//static PFNGLBINDFRAMEBUFFERPROC s_pFn_glBindFramebuffer = NULL;
//VS TES TCS GS PS
static PFNGLUSEPROGRAMPROC s_pFn_glUseProgram = NULL;
//static PFNGLBINDBUFFERPROC s_pFn_glBindBuffer = NULL;
static PFNGLBINDBUFFERBASEPROC s_pFn_glBindBufferBase = NULL;
//static PFNGLACTIVETEXTUREPROC s_pFn_glActiveTexture = NULL;
//static PFNGLBINDTEXTUREPROC s_pFn_glBindTexture = NULL;
static PFNGLBINDSAMPLERPROC s_pFn_glBindSampler = NULL;
static PFNGLBINDIMAGETEXTUREPROC s_pFn_glBindImageTexture = NULL;
//RS
static PFNGLCLIPCONTROLEXTPROC s_pFn_glClipControlEXT = NULL;
static PFNGLVIEWPORTARRAYVOESPROC s_pFn_glViewportArrayvOES = NULL;
static PFNGLPOLYGONOFFSETCLAMPEXTPROC s_pFn_glPolygonOffsetClampEXT = NULL;
static PFNGLPOLYGONMODENVPROC s_pFn_glPolygonModeNV = NULL;
static PFNGLFRONTFACEPROC s_pFn_glFrontFace = NULL;
static PFNGLCULLFACEPROC s_pFn_glCullFace = NULL;

//IA
//static PFNGLBINDVERTEXARRAYPROC s_pFn_glBindVertexArray = NULL;
static PFNGLBINDVERTEXBUFFERPROC s_pFn_glBindVertexBuffer = NULL;
static PFNGLPATCHPARAMETERIEXTPROC s_pFn_glPatchParameteriOES = NULL;
//glPrimitiveRestartIndex
//glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX)
static PFNGLDRAWARRAYSPROC s_pFn_glDrawArrays = NULL;
static PFNGLDRAWELEMENTSPROC s_pFn_glDrawElements = NULL;
static PFNGLDRAWELEMENTSINSTANCEDPROC s_pFn_glDrawElementsInstanced = NULL;
//OM
static PFNGLDEPTHFUNCPROC s_pFn_glDepthFunc = NULL;
static PFNGLBLENDFUNCSEPARATEIOESPROC s_pFn_glBlendFuncSeparateiOES = NULL;
static PFNGLBLENDEQUATIONSEPARATEIOESPROC s_pFn_glBlendEquationSeparateiOES = NULL;

//Clear
static PFNGLCLEARBUFFERFVPROC s_pFn_glClearBufferfv = NULL;
//Resolve
static PFNGLBLITFRAMEBUFFERPROC s_pFn_glBlitFramebuffer = NULL;

//
static PFNGLDISPATCHCOMPUTEPROC s_pFn_glDispatchCompute = NULL;

//Copy
static PFNGLCOPYIMAGESUBDATAOESPROC s_pFn_glCopyImageSubDataOES = NULL;

extern "C" EGLBoolean EGLAPIENTRY eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor)
{
	//Create Temp DC Because SetPixelFormat Can't Change
	HDC hDC1 = static_cast<EGLNativeDisplayType>(dpy);

	BOOL wbResult;

	HWND hWnd1 = ::WindowFromDC(hDC1);
	assert(hWnd1 != NULL);

	HWND hWndParent = ::GetAncestor(hWnd1,GA_PARENT);
	assert(hWndParent != NULL);

	HMONITOR hMonitor = ::MonitorFromWindow(hWndParent, MONITOR_DEFAULTTONEAREST);
	assert(hMonitor != NULL);

	MONITORINFOEXW MonitorInfo;
	MonitorInfo.cbSize = sizeof(MONITORINFOEXW);
	wbResult = ::GetMonitorInfoW(hMonitor, &MonitorInfo);
	assert(wbResult != FALSE);

	HMODULE hDlllibEGL = ::GetModuleHandleW(L"libEGL");

	WNDCLASSEXW Desc = {
		sizeof(WNDCLASSEX),
		CS_OWNDC,
		[](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)->LRESULT
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	},
		0,
		0,
		hDlllibEGL,
		LoadIconW(NULL, IDI_APPLICATION),
		LoadCursorW(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		L"eglInitialize:0XFFFFFFFF",
		LoadIconW(NULL, IDI_APPLICATION),
	};
	ATOM hWndTempClass = ::RegisterClassExW(&Desc);
	assert(hWndTempClass != 0U);

	HWND hWndTemp = ::CreateWindowExW(WS_EX_APPWINDOW,
		MAKEINTATOM(hWndTempClass),
		L"eglInitialize:0XFFFFFFFF",
		WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		MonitorInfo.rcWork.left,
		MonitorInfo.rcWork.top,
		MonitorInfo.rcWork.right - MonitorInfo.rcWork.left,
		MonitorInfo.rcWork.bottom - MonitorInfo.rcWork.top,
		hWndParent,
		NULL,
		hDlllibEGL,
		NULL);
	assert(hWndTemp != NULL);

	HDC hDCTemp = ::GetDC(hWndTemp);
	assert(hDCTemp != NULL);

	PIXELFORMATDESCRIPTOR FormatDescriptor;
	int PixelFormatCount = ::DescribePixelFormat(hDCTemp, 1, sizeof(FormatDescriptor), &FormatDescriptor);//Load OpenGL ICD

	int PixelFormatIndex = 1;
	while (PixelFormatIndex <= PixelFormatCount)//从1开始
	{
		int iResult = ::DescribePixelFormat(hDCTemp, PixelFormatIndex, sizeof(FormatDescriptor), &FormatDescriptor);//Load OpenGL ICD
		assert(iResult != 0);

		if (((FormatDescriptor.dwFlags&(PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL)) == (PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL))
			&& (FormatDescriptor.iPixelType == PFD_TYPE_RGBA))
		{
			break;
		}

		++PixelFormatIndex;
	}

	wbResult = ::SetPixelFormat(hDCTemp, PixelFormatIndex, &FormatDescriptor);
	assert(wbResult != FALSE);

	HGLRC hWGLContext = ::wglCreateContext(hDCTemp);
	assert(hWGLContext != NULL);

	wbResult = ::wglMakeCurrent(hDCTemp, hWGLContext);
	assert(wbResult != FALSE);

	s_pFn_wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(::wglGetProcAddress("wglCreateContextAttribsARB"));
	assert(s_pFn_wglCreateContextAttribsARB != NULL);

	wbResult = ::wglMakeCurrent(NULL, NULL);
	assert(wbResult != FALSE);

	wbResult = ::wglDeleteContext(hWGLContext);
	assert(wbResult != FALSE);

	HGLRC hWGLContextARB;
	{
#ifdef ESPROFILE
		int attribList[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB,3,
			WGL_CONTEXT_MINOR_VERSION_ARB,1,
			WGL_CONTEXT_PROFILE_MASK_ARB,WGL_CONTEXT_ES_PROFILE_BIT_EXT,//OpenGL ES
			WGL_CONTEXT_FLAGS_ARB ,WGL_CONTEXT_DEBUG_BIT_ARB,
			0
		};
#else
		int attribList[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB,4,
			WGL_CONTEXT_MINOR_VERSION_ARB,5,
			WGL_CONTEXT_PROFILE_MASK_ARB,WGL_CONTEXT_CORE_PROFILE_BIT_ARB,//RenderDoc不支持OpenGL ES
			WGL_CONTEXT_FLAGS_ARB ,WGL_CONTEXT_DEBUG_BIT_ARB,
			0
		};
#endif
		hWGLContextARB = s_pFn_wglCreateContextAttribsARB(hDCTemp, NULL, attribList);
	}

	wbResult = ::wglMakeCurrent(hDCTemp, hWGLContextARB);
	assert(wbResult != FALSE);

	//OpenGL ES 3.1
	//GetProcAddress
	//wglGetProcAddress
	HMODULE hDllOpenGL32 = ::GetModuleHandleW(L"OpenGL32");
	assert(hDllOpenGL32 != NULL);

	s_pFn_wglMakeContextCurrentARB = reinterpret_cast<PFNWGLMAKECONTEXTCURRENTARBPROC>(::wglGetProcAddress("wglMakeContextCurrentARB"));
	if (s_pFn_wglMakeContextCurrentARB == NULL)
	{
		s_pFn_wglMakeContextCurrentARB = reinterpret_cast<PFNWGLMAKECONTEXTCURRENTEXTPROC>(::wglGetProcAddress("wglMakeContextCurrentEXT"));
	}
	assert(s_pFn_wglMakeContextCurrentARB != NULL);

	s_pFn_wglSwapIntervalEXT = reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>(::wglGetProcAddress("wglSwapIntervalEXT"));
	assert(s_pFn_wglSwapIntervalEXT != NULL);

	s_pFn_glDebugMessageControlKHR = reinterpret_cast<PFNGLDEBUGMESSAGECONTROLKHRPROC>(::wglGetProcAddress("glDebugMessageControlKHR"));//OpenGL ES GL_KHR_debug
	if (s_pFn_glDebugMessageControlKHR == NULL)
	{
		s_pFn_glDebugMessageControlKHR = reinterpret_cast<PFNGLDEBUGMESSAGECONTROLKHRPROC>(::wglGetProcAddress("glDebugMessageControl"));//OpenGL ES 3.2
	}
	assert(s_pFn_glDebugMessageControlKHR != NULL);
	s_pFn_glDebugMessageCallbackKHR = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKKHRPROC>(::wglGetProcAddress("glDebugMessageCallbackKHR"));//OpenGL ES GL_KHR_debug
	if (s_pFn_glDebugMessageCallbackKHR == NULL)
	{
		s_pFn_glDebugMessageCallbackKHR = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKKHRPROC>(::wglGetProcAddress("glDebugMessageCallback"));//OpenGL ES 3.2
	}
	assert(s_pFn_glDebugMessageCallbackKHR != NULL);

	s_pFn_glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(::wglGetProcAddress("glGenBuffers"));
	assert(s_pFn_glGenBuffers != NULL);
	s_pFn_glGenTextures = reinterpret_cast<PFNGLGENTEXTURESPROC>(::GetProcAddress(hDllOpenGL32, "glGenTextures"));
	assert(s_pFn_glGenTextures != NULL);

	s_pFn_glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(::wglGetProcAddress("glBindBuffer"));
	assert(s_pFn_glBindBuffer != NULL);
	s_pFn_glActiveTexture = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(::wglGetProcAddress("glActiveTexture"));
	assert(s_pFn_glActiveTexture != NULL);
	s_pFn_glBindTexture = reinterpret_cast<PFNGLBINDTEXTUREPROC>(::GetProcAddress(hDllOpenGL32, "glBindTexture"));
	assert(s_pFn_glBindTexture != NULL);

	s_pFn_glBufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>(::wglGetProcAddress("glBufferData"));
	assert(s_pFn_glBufferData != NULL);
	s_pFn_glTexBufferOES = reinterpret_cast<PFNGLTEXBUFFEROESPROC>(::wglGetProcAddress("glTexBufferOES"));//GL_OES_texture_buffer
	if (s_pFn_glTexBufferOES == NULL)
	{
		s_pFn_glTexBufferOES = reinterpret_cast<PFNGLTEXBUFFEROESPROC>(::wglGetProcAddress("glTexBufferEXT"));//GL_EXT_texture_buffer
	}
	if (s_pFn_glTexBufferOES == NULL)
	{
		s_pFn_glTexBufferOES = reinterpret_cast<PFNGLTEXBUFFEROESPROC>(::wglGetProcAddress("glTexBuffer"));//OpenGL Core
	}
	assert(s_pFn_glTexBufferOES != NULL);
	s_pFn_glTexStorage2D = reinterpret_cast<PFNGLTEXSTORAGE2DPROC>(::wglGetProcAddress("glTexStorage2D"));
	assert(s_pFn_glTexStorage2D != NULL);
	s_pFn_glTexStorage3D = reinterpret_cast<PFNGLTEXSTORAGE3DPROC>(::wglGetProcAddress("glTexStorage3D"));
	assert(s_pFn_glTexStorage3D != NULL);

	s_pFn_glTexStorage2DMultisample = reinterpret_cast<PFNGLTEXSTORAGE2DMULTISAMPLEPROC>(::wglGetProcAddress("glTexStorage2DMultisample"));
	assert(s_pFn_glTexStorage2DMultisample != NULL);

	s_pFn_glCompressedTexImage3DOES = reinterpret_cast<PFNGLCOMPRESSEDTEXIMAGE3DOESPROC>(::wglGetProcAddress("glCompressedTexImage3DOES"));//GL_OES_texture_3D
	if (s_pFn_glCompressedTexImage3DOES == NULL)
	{
		s_pFn_glCompressedTexImage3DOES = reinterpret_cast<PFNGLCOMPRESSEDTEXIMAGE3DPROC>(::wglGetProcAddress("glCompressedTexImage3D"));//OpenGL Core
	}
	assert(s_pFn_glCompressedTexImage3DOES != NULL);

	s_pFn_glBufferSubData = reinterpret_cast<PFNGLBUFFERSUBDATAPROC>(::wglGetProcAddress("glBufferSubData"));
	assert(s_pFn_glBufferSubData != NULL);
	s_pFn_glTexSubImage2D = reinterpret_cast<PFNGLTEXSUBIMAGE2DPROC>(::wglGetProcAddress("glTexSubImage2D"));
	assert(s_pFn_glTexSubImage2D != NULL);
	s_pFn_glTexSubImage3D = reinterpret_cast<PFNGLTEXSUBIMAGE3DPROC>(::wglGetProcAddress("glTexSubImage3D"));
	assert(s_pFn_glTexSubImage3D != NULL);

	s_pFn_glGenSamplers = reinterpret_cast<PFNGLGENSAMPLERSPROC>(::wglGetProcAddress("glGenSamplers"));
	assert(s_pFn_glGenSamplers != NULL);
	s_pFn_glSamplerParameteri = reinterpret_cast<PFNGLSAMPLERPARAMETERIPROC>(::wglGetProcAddress("glSamplerParameteri"));
	assert(s_pFn_glSamplerParameteri != NULL);
	s_pFn_glSamplerParameterfv = reinterpret_cast<PFNGLSAMPLERPARAMETERFVPROC>(::wglGetProcAddress("glSamplerParameterfv"));
	assert(s_pFn_glSamplerParameterfv != NULL);

	s_pFn_glGenFramebuffers = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(::wglGetProcAddress("glGenFramebuffers"));
	assert(s_pFn_glGenFramebuffers != NULL);
	s_pFn_glBindFramebuffer = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(::wglGetProcAddress("glBindFramebuffer"));
	assert(s_pFn_glBindFramebuffer != NULL);
	s_pFn_glFramebufferTexture2D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(::wglGetProcAddress("glFramebufferTexture2D"));
	assert(s_pFn_glFramebufferTexture2D != NULL);
	s_pFn_glReadBuffer = reinterpret_cast<PFNGLREADBUFFERPROC>(::GetProcAddress(hDllOpenGL32, "glReadBuffer"));
	assert(s_pFn_glReadBuffer != NULL);
	s_pFn_glDrawBuffers = reinterpret_cast<PFNGLDRAWBUFFERSPROC>(::wglGetProcAddress("glDrawBuffers"));
	assert(s_pFn_glDrawBuffers != NULL);
	s_pFn_glCheckFramebufferStatus = reinterpret_cast<PFNGLCHECKFRAMEBUFFERSTATUSPROC>(::wglGetProcAddress("glCheckFramebufferStatus"));
	assert(s_pFn_glCheckFramebufferStatus != NULL);

	s_pFn_glCreateShader = reinterpret_cast<PFNGLCREATESHADERPROC>(::wglGetProcAddress("glCreateShader"));
	assert(s_pFn_glCreateShader != NULL);
	s_pFn_glShaderSource = reinterpret_cast<PFNGLSHADERSOURCEPROC>(::wglGetProcAddress("glShaderSource"));
	assert(s_pFn_glShaderSource != NULL);
	s_pFn_glCompileShader = reinterpret_cast<PFNGLCOMPILESHADERPROC>(::wglGetProcAddress("glCompileShader"));
	assert(s_pFn_glCompileShader != NULL);
	s_pFn_glGetShaderiv = reinterpret_cast<PFNGLGETSHADERIVPROC>(::wglGetProcAddress("glGetShaderiv"));
	assert(s_pFn_glGetShaderiv != NULL);
	s_pFn_glGetShaderInfoLog = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(::wglGetProcAddress("glGetShaderInfoLog"));
	assert(s_pFn_glGetShaderInfoLog != NULL);

	s_pFn_glCreateProgram = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(::wglGetProcAddress("glCreateProgram"));
	assert(s_pFn_glCreateProgram != NULL);
	s_pFn_glAttachShader = reinterpret_cast<PFNGLATTACHSHADERPROC>(::wglGetProcAddress("glAttachShader"));
	assert(s_pFn_glAttachShader != NULL);
	s_pFn_glLinkProgram = reinterpret_cast<PFNGLLINKPROGRAMPROC>(::wglGetProcAddress("glLinkProgram"));
	assert(s_pFn_glLinkProgram != NULL);
	s_pFn_glGetProgramiv = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(::wglGetProcAddress("glGetProgramiv"));
	assert(s_pFn_glGetProgramiv != NULL);
	s_pFn_glGetProgramInfoLog = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(::wglGetProcAddress("glGetProgramInfoLog"));
	assert(s_pFn_glGetProgramInfoLog != NULL);

	s_pFn_glGenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(::wglGetProcAddress("glGenVertexArrays"));
	assert(s_pFn_glGenVertexArrays != NULL);
	s_pFn_glBindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(::wglGetProcAddress("glBindVertexArray"));
	assert(s_pFn_glBindVertexArray != NULL);
	s_pFn_glVertexAttribFormat = reinterpret_cast<PFNGLVERTEXATTRIBFORMATPROC>(::wglGetProcAddress("glVertexAttribFormat"));
	assert(s_pFn_glVertexAttribFormat != NULL);
	s_pFn_glEnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(::wglGetProcAddress("glEnableVertexAttribArray"));
	assert(s_pFn_glEnableVertexAttribArray != NULL);
	s_pFn_glVertexAttribBinding = reinterpret_cast<PFNGLVERTEXATTRIBBINDINGPROC>(::wglGetProcAddress("glVertexAttribBinding"));
	assert(s_pFn_glVertexAttribBinding != NULL);

	s_pFn_glEnable = reinterpret_cast<PFNGLENABLEPROC>(::GetProcAddress(hDllOpenGL32, "glEnable"));
	assert(s_pFn_glEnable != NULL);
	s_pFn_glDisable = reinterpret_cast<PFNGLENABLEPROC>(::GetProcAddress(hDllOpenGL32, "glDisable"));
	assert(s_pFn_glDisable != NULL);

	s_pFn_glUseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(::wglGetProcAddress("glUseProgram"));
	assert(s_pFn_glUseProgram != NULL);
	s_pFn_glBindBufferBase = reinterpret_cast<PFNGLBINDBUFFERBASEPROC>(::wglGetProcAddress("glBindBufferBase"));
	assert(s_pFn_glBindBufferBase != NULL);
	s_pFn_glBindSampler = reinterpret_cast<PFNGLBINDSAMPLERPROC>(::wglGetProcAddress("glBindSampler"));
	assert(s_pFn_glBindSampler != NULL);
	s_pFn_glBindImageTexture = reinterpret_cast<PFNGLBINDIMAGETEXTUREPROC>(::wglGetProcAddress("glBindImageTexture"));
	assert(s_pFn_glBindImageTexture != NULL);

	s_pFn_glClipControlEXT = reinterpret_cast<PFNGLCLIPCONTROLEXTPROC>(::wglGetProcAddress("glClipControlEXT"));//OpenGL ES GL_EXT_clip_control
	if (s_pFn_glClipControlEXT == NULL)
	{
		s_pFn_glClipControlEXT = reinterpret_cast<PFNGLCLIPCONTROLEXTPROC>(::wglGetProcAddress("glClipControl"));//OpenGL Core GL_ARB_clip_control
	}
	assert(s_pFn_glClipControlEXT != NULL);

	s_pFn_glViewportArrayvOES = reinterpret_cast<PFNGLVIEWPORTARRAYVOESPROC>(::wglGetProcAddress("glViewportArrayvOES"));//OpenGL ES GL_OES_viewport_array
	if (s_pFn_glClipControlEXT == NULL)
	{
		s_pFn_glViewportArrayvOES = reinterpret_cast<PFNGLVIEWPORTARRAYVOESPROC>(::wglGetProcAddress("glViewportArrayv"));//OpenGL Core GL_ARB_viewport_array
	}
	assert(s_pFn_glViewportArrayvOES != NULL);

	s_pFn_glPolygonOffsetClampEXT = reinterpret_cast<PFNGLPOLYGONOFFSETCLAMPEXTPROC>(::wglGetProcAddress("glPolygonOffsetClampEXT"));//OpenGL ES GL_EXT_polygon_offset_clamp
	if (s_pFn_glPolygonOffsetClampEXT == NULL)
	{
		s_pFn_glPolygonOffsetClampEXT = reinterpret_cast<PFNGLPOLYGONOFFSETCLAMPEXTPROC>(::wglGetProcAddress("glPolygonOffsetClamp"));//OpenGL Core 4.6
	}
	assert(s_pFn_glPolygonOffsetClampEXT != NULL);

	s_pFn_glPolygonModeNV = reinterpret_cast<PFNGLPOLYGONMODENVPROC>(::wglGetProcAddress("glPolygonModeNV"));//OpenGL ES GL_NV_polygon_mode
	if (s_pFn_glPolygonModeNV == NULL)
	{
		s_pFn_glPolygonModeNV = reinterpret_cast<PFNGLPOLYGONMODENVPROC>(::wglGetProcAddress("glPolygonMode"));//OpenGL Core 1.0
	}
	assert(s_pFn_glPolygonModeNV != NULL);

	s_pFn_glFrontFace = reinterpret_cast<PFNGLFRONTFACEPROC>(::GetProcAddress(hDllOpenGL32, "glFrontFace"));
	assert(s_pFn_glFrontFace != NULL);
	s_pFn_glCullFace = reinterpret_cast<PFNGLCULLFACEPROC>(::GetProcAddress(hDllOpenGL32, "glCullFace"));
	assert(s_pFn_glCullFace != NULL);

	s_pFn_glBindVertexBuffer = reinterpret_cast<PFNGLBINDVERTEXBUFFERPROC>(::wglGetProcAddress("glBindVertexBuffer"));
	assert(s_pFn_glBindVertexBuffer != NULL);
	
	s_pFn_glPatchParameteriOES = reinterpret_cast<PFNGLPATCHPARAMETERIOESPROC>(::wglGetProcAddress("glPatchParameteriOES"));//OpenGL ES GL_OES_tessellation_shader
	if (s_pFn_glPatchParameteriOES == NULL)
	{
		s_pFn_glPatchParameteriOES = reinterpret_cast<PFNGLPATCHPARAMETERIEXTPROC>(::wglGetProcAddress("glPatchParameteriEXT"));//OpenGL ES GL_EXT_tessellation_shader
	}
	if (s_pFn_glPatchParameteriOES == NULL)
	{
		s_pFn_glPatchParameteriOES = reinterpret_cast<PFNGLPATCHPARAMETERIOESPROC>(::wglGetProcAddress("glPatchParameteri"));//OpenGL ES 3.2 
	}
	assert(s_pFn_glPatchParameteriOES != NULL);

	s_pFn_glDrawArrays = reinterpret_cast<PFNGLDRAWARRAYSPROC>(::GetProcAddress(hDllOpenGL32, "glDrawArrays"));
	assert(s_pFn_glDrawArrays != NULL);
	s_pFn_glDrawElements = reinterpret_cast<PFNGLDRAWELEMENTSPROC>(::GetProcAddress(hDllOpenGL32, "glDrawElements"));
	assert(s_pFn_glDrawElements != NULL);
	s_pFn_glDrawElementsInstanced = reinterpret_cast<PFNGLDRAWELEMENTSINSTANCEDPROC>(::wglGetProcAddress("glDrawElementsInstanced"));
	assert(s_pFn_glDrawElementsInstanced != NULL);
	s_pFn_glDepthFunc = reinterpret_cast<PFNGLDEPTHFUNCPROC>(::GetProcAddress(hDllOpenGL32, "glDepthFunc"));
	assert(s_pFn_glDepthFunc != NULL);

	s_pFn_glBlendFuncSeparateiOES = reinterpret_cast<PFNGLBLENDFUNCSEPARATEIOESPROC>(::wglGetProcAddress("glBlendFuncSeparateiOES"));
	if (s_pFn_glBlendFuncSeparateiOES == NULL)
	{
		s_pFn_glBlendFuncSeparateiOES = reinterpret_cast<PFNGLBLENDFUNCSEPARATEIEXTPROC>(::wglGetProcAddress("glBlendFuncSeparateiEXT"));
	}
	if (s_pFn_glBlendFuncSeparateiOES == NULL)
	{
		s_pFn_glBlendFuncSeparateiOES = reinterpret_cast<PFNGLBLENDFUNCSEPARATEIOESPROC>(::wglGetProcAddress("glBlendFuncSeparatei"));//OpenGL Core 4.0
	}
	assert(s_pFn_glBlendFuncSeparateiOES != NULL);

	s_pFn_glBlendEquationSeparateiOES = reinterpret_cast<PFNGLBLENDEQUATIONSEPARATEIOESPROC>(::wglGetProcAddress("glBlendEquationSeparateiOES"));
	if (s_pFn_glBlendEquationSeparateiOES == NULL)
	{
		s_pFn_glBlendEquationSeparateiOES = reinterpret_cast<PFNGLBLENDEQUATIONSEPARATEIEXTPROC>(::wglGetProcAddress("glBlendEquationSeparateiEXT"));
	}
	if (s_pFn_glBlendEquationSeparateiOES == NULL)
	{
		s_pFn_glBlendEquationSeparateiOES = reinterpret_cast<PFNGLBLENDEQUATIONSEPARATEIOESPROC>(::wglGetProcAddress("glBlendEquationSeparatei"));//OpenGL Core 4.0
	}
	assert(s_pFn_glBlendEquationSeparateiOES != NULL);

	s_pFn_glClearBufferfv = reinterpret_cast<PFNGLCLEARBUFFERFVPROC>(::wglGetProcAddress("glClearBufferfv"));
	assert(s_pFn_glClearBufferfv != NULL);
	s_pFn_glBlitFramebuffer = reinterpret_cast<PFNGLBLITFRAMEBUFFERPROC>(::wglGetProcAddress("glBlitFramebuffer"));
	assert(s_pFn_glBlitFramebuffer != NULL);

	s_pFn_glDispatchCompute = reinterpret_cast<PFNGLDISPATCHCOMPUTEPROC>(::wglGetProcAddress("glDispatchCompute"));
	assert(s_pFn_glDispatchCompute != NULL);

	s_pFn_glCopyImageSubDataOES = reinterpret_cast<PFNGLCOPYIMAGESUBDATAOESPROC>(::wglGetProcAddress("glCopyImageSubDataOES"));
	if (s_pFn_glCopyImageSubDataOES == NULL)
	{
		s_pFn_glCopyImageSubDataOES = reinterpret_cast<PFNGLCOPYIMAGESUBDATAEXTPROC>(::wglGetProcAddress("glCopyImageSubDataEXT"));
	}
	if (s_pFn_glCopyImageSubDataOES == NULL)
	{
		s_pFn_glCopyImageSubDataOES = reinterpret_cast<PFNGLCOPYIMAGESUBDATAOESPROC>(::wglGetProcAddress("glCopyImageSubData"));
	}
	assert(s_pFn_glCopyImageSubDataOES != NULL);

	wbResult = ::wglMakeCurrent(NULL, NULL);
	assert(wbResult != FALSE);

	wbResult = ::wglDeleteContext(hWGLContextARB);
	assert(wbResult != FALSE);

	wbResult = ::DestroyWindow(hWndTemp);
	assert(wbResult != FALSE);

	wbResult = ::UnregisterClassW(L"eglInitialize:0XFFFFFFFF", hDlllibEGL);
	assert(wbResult != FALSE);

	//EGL 1.4
	if (major != NULL)
	{
		(*major) = 1;
	}
	if (minor != NULL)
	{
		(*minor) = 4;
	}
	return EGL_TRUE;
}

extern "C" EGLBoolean EGLAPIENTRY eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
	EGLBoolean eglbResult;

	//Attrib
	std::map<EGLint, EGLint> l_ConfigReferenceAttribMap;
	for (const EGLint *pAttrib = attrib_list; pAttrib[0] != EGL_NONE; pAttrib += 2)
	{
		l_ConfigReferenceAttribMap[pAttrib[0]] = pAttrib[1];
	}

	//All
	std::vector<EGLConfig> l_ConfigToChooseArray;
	{
		EGLint l_Num_Config1;
		EGLint l_Num_Config2;

		eglbResult = ::eglGetConfigs(dpy, NULL, 0, &l_Num_Config1);
		assert(eglbResult == EGL_TRUE);

		l_ConfigToChooseArray.resize(l_Num_Config1);

		eglbResult = ::eglGetConfigs(dpy, &l_ConfigToChooseArray[0], l_Num_Config1, &l_Num_Config2);
		assert(eglbResult == EGL_TRUE);

		assert(l_Num_Config1 == l_Num_Config2);
	}

	//Selction
	std::vector<EGLConfig> l_ConfigChoosenArray;
	for (std::vector<EGLConfig>::const_reference rConfigToChoose : l_ConfigToChooseArray)
	{
		bool match = true;

		for (std::map<EGLint, EGLint>::const_reference rAttribReference : l_ConfigReferenceAttribMap)
		{
			EGLint l_ConfigToChooseValue;
			eglbResult = ::eglGetConfigAttrib(dpy, rConfigToChoose, rAttribReference.first, &l_ConfigToChooseValue);
			assert(eglbResult == EGL_TRUE);

			switch (rAttribReference.first)
			{
			case EGL_CONFIG_ID: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_CONFIG_CAVEAT: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_COLOR_BUFFER_TYPE: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_RED_SIZE: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			case EGL_GREEN_SIZE: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			case EGL_BLUE_SIZE: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			case EGL_LUMINANCE_SIZE: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			case EGL_ALPHA_SIZE: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			case EGL_BUFFER_SIZE: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			case EGL_SAMPLE_BUFFERS: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			case EGL_SAMPLES: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			case EGL_DEPTH_SIZE: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			case EGL_STENCIL_SIZE: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			case EGL_ALPHA_MASK_SIZE: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			case EGL_SURFACE_TYPE:  match = ((l_ConfigToChooseValue&rAttribReference.second) == rAttribReference.second); break;
			case EGL_RENDERABLE_TYPE: match = ((l_ConfigToChooseValue&rAttribReference.second) == rAttribReference.second); break;
			case EGL_CONFORMANT: match = ((l_ConfigToChooseValue&rAttribReference.second) == rAttribReference.second); break;
			case EGL_MIN_SWAP_INTERVAL: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_MAX_SWAP_INTERVAL: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_TRANSPARENT_TYPE: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_TRANSPARENT_BLUE_VALUE: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_TRANSPARENT_GREEN_VALUE: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_TRANSPARENT_RED_VALUE: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_NATIVE_RENDERABLE: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_NATIVE_VISUAL_TYPE: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_LEVEL: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_BIND_TO_TEXTURE_RGB: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_BIND_TO_TEXTURE_RGBA: match = (l_ConfigToChooseValue == rAttribReference.second); break;
			case EGL_MAX_PBUFFER_WIDTH: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			case EGL_MAX_PBUFFER_HEIGHT: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			case EGL_MAX_PBUFFER_PIXELS: match = (l_ConfigToChooseValue >= rAttribReference.second); break;
			}

			if (!match)
			{
				break;
			}
		}

		if (match)
		{
			l_ConfigChoosenArray.push_back(rConfigToChoose);
		}
	}

	//sort
	std::sort(l_ConfigChoosenArray.begin(), l_ConfigChoosenArray.end(), [dpy](EGLConfig const &rConfigX, EGLConfig const &rConfigY)->bool {
		EGLBoolean eglbResult;
		EGLint l_AttribX;
		EGLint l_AttribY;

		//EGL_CONFIG_CAVEAT
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigX, EGL_CONFIG_CAVEAT, &l_AttribX);
		assert(eglbResult == EGL_TRUE);
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigY, EGL_CONFIG_CAVEAT, &l_AttribY);
		assert(eglbResult == EGL_TRUE);
		assert(l_AttribX == EGL_NONE);
		assert(l_AttribY == EGL_NONE);

		//EGL_CONFIG_CAVEAT
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigX, EGL_CONFIG_CAVEAT, &l_AttribX);
		assert(eglbResult == EGL_TRUE);
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigY, EGL_CONFIG_CAVEAT, &l_AttribY);
		assert(eglbResult == EGL_TRUE);
		assert(l_AttribX == EGL_NONE);
		assert(l_AttribY == EGL_NONE);

		//EGL_COLOR_BUFFER_TYPE
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigX, EGL_COLOR_BUFFER_TYPE, &l_AttribX);
		assert(eglbResult == EGL_TRUE);
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigY, EGL_COLOR_BUFFER_TYPE, &l_AttribY);
		assert(eglbResult == EGL_TRUE);
		assert(l_AttribX == EGL_RGB_BUFFER);
		assert(l_AttribY == EGL_RGB_BUFFER);

		//EGL_RED_SIZE EGL_GREEN_SIZE EGL_BLUE_SIZE EGL_ALPHA_SIZE / EGL_LUMINANCE_SIZE + EGL_ALPHA_SIZE
		
		//EGL_BUFFER_SIZE
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigX, EGL_BUFFER_SIZE, &l_AttribX);
		assert(eglbResult == EGL_TRUE);
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigY, EGL_BUFFER_SIZE, &l_AttribY);
		assert(eglbResult == EGL_TRUE);
		if (l_AttribX != l_AttribY)
		{
			return l_AttribX <l_AttribY;
		}

		//EGL_SAMPLE_BUFFERS
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigX, EGL_SAMPLE_BUFFERS, &l_AttribX);
		assert(eglbResult == EGL_TRUE);
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigY, EGL_SAMPLE_BUFFERS, &l_AttribY);
		assert(eglbResult == EGL_TRUE);
		assert(l_AttribX == 0);
		assert(l_AttribY == 0);

		//EGL_SAMPLES
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigX, EGL_SAMPLES, &l_AttribX);
		assert(eglbResult == EGL_TRUE);
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigY, EGL_SAMPLES, &l_AttribY);
		assert(eglbResult == EGL_TRUE);
		assert(l_AttribX == 0);
		assert(l_AttribY == 0);

		//EGL_DEPTH_SIZE
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigX, EGL_DEPTH_SIZE, &l_AttribX);
		assert(eglbResult == EGL_TRUE);
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigY, EGL_DEPTH_SIZE, &l_AttribY);
		assert(eglbResult == EGL_TRUE);
		if (l_AttribX != l_AttribY)
		{
			return l_AttribX <l_AttribY;
		}

		//EGL_STENCIL_SIZE
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigX, EGL_STENCIL_SIZE, &l_AttribX);
		assert(eglbResult == EGL_TRUE);
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigY, EGL_STENCIL_SIZE, &l_AttribY);
		assert(eglbResult == EGL_TRUE);
		if (l_AttribX != l_AttribY)
		{
			return l_AttribX <l_AttribY;
		}

		//EGL_ALPHA_MASK_SIZE
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigX, EGL_ALPHA_MASK_SIZE, &l_AttribX);
		assert(eglbResult == EGL_TRUE);
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigY, EGL_ALPHA_MASK_SIZE, &l_AttribY);
		assert(eglbResult == EGL_TRUE);
		assert(l_AttribX == 0);
		assert(l_AttribY == 0);

		//EGL_CONFIG_ID
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigX, EGL_CONFIG_ID, &l_AttribX);
		assert(eglbResult == EGL_TRUE);
		eglbResult = ::eglGetConfigAttrib(dpy, rConfigY, EGL_CONFIG_ID, &l_AttribY);
		assert(eglbResult == EGL_TRUE);
		assert(l_AttribX != l_AttribY);
		return l_AttribX <l_AttribY;
	});

	//configs
	if (configs)
	{
		l_ConfigChoosenArray.resize(std::min<size_t>(l_ConfigChoosenArray.size(), config_size));
		for (int i = 0; i < l_ConfigChoosenArray.size(); ++i)
		{
			configs[i] = l_ConfigChoosenArray[i];
		}
	}

	//num_config
	if (num_config == NULL)
	{
		s_EGLError = EGL_BAD_PARAMETER;
		return EGL_FALSE;
	}
	(*num_config) = static_cast<EGLint>(l_ConfigChoosenArray.size());

	return EGL_TRUE;
}

extern "C" EGLBoolean EGLAPIENTRY eglGetConfigs(EGLDisplay dpy, EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
	//dpy
	HDC hDC = static_cast<EGLNativeDisplayType>(dpy);

	//
	std::vector<EGLConfig> l_ConfigArray;

	PIXELFORMATDESCRIPTOR FormatDescriptor;
	int PixelFormatCount = ::DescribePixelFormat(hDC, 1, sizeof(FormatDescriptor), &FormatDescriptor);
	
	for (int i = 0; i < PixelFormatCount; ++i)
	{
		int iResult = ::DescribePixelFormat(hDC, 1 + i, sizeof(FormatDescriptor), &FormatDescriptor);//从1开始
		assert(iResult != 0);

		if (
			((FormatDescriptor.dwFlags&(PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL)) == (PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL))
			&& (FormatDescriptor.iPixelType == PFD_TYPE_RGBA)
			)
		{
			l_ConfigArray.push_back(reinterpret_cast<EGLConfig>(static_cast<intptr_t>(1 + i)));//从1开始
		}
	}

	//configs
	if (configs)
	{
		l_ConfigArray.resize(std::min<size_t>(l_ConfigArray.size(), config_size));
		for (int i = 0; i < l_ConfigArray.size(); ++i)
		{
			configs[i] = l_ConfigArray[i];
		}
	}

	//num_config
	if (num_config == NULL)
	{
		s_EGLError = EGL_BAD_PARAMETER;
		return EGL_FALSE;
	}
	(*num_config) = static_cast<EGLint>(l_ConfigArray.size());
	
	return EGL_TRUE;
}

extern "C" EGLBoolean EGLAPIENTRY eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value)
{
	//dpy
	HDC hDC = static_cast<EGLNativeDisplayType>(dpy);

	//config
	int PixelFormatIndex = static_cast<int>(reinterpret_cast<intptr_t>(config));

	PIXELFORMATDESCRIPTOR FormatDescriptor;
	int iResult = ::DescribePixelFormat(hDC, PixelFormatIndex, sizeof(FormatDescriptor), &FormatDescriptor);
	assert(iResult != 0);

	switch (attribute)
	{
	//ID
	case EGL_CONFIG_ID:
		(*value) = static_cast<int>(reinterpret_cast<intptr_t>(config));//PixelFormatIndex
		break;
	//Color
	case EGL_CONFIG_CAVEAT:
		(*value) = EGL_NONE;
		break;
	//RGB
	case EGL_COLOR_BUFFER_TYPE:
		assert(FormatDescriptor.iPixelType == PFD_TYPE_RGBA);
		(*value) = EGL_RGB_BUFFER;
		break;
	case EGL_RED_SIZE:
		(*value) = FormatDescriptor.cRedBits;
		break;
	case EGL_GREEN_SIZE:
		(*value) = FormatDescriptor.cGreenBits;
		break;
	case EGL_BLUE_SIZE:
		(*value) = FormatDescriptor.cBlueBits;
		break;
	//Luminance
	case EGL_LUMINANCE_SIZE:
		(*value) = 0;
		break;
	//Alpha
	case EGL_ALPHA_SIZE:
		(*value) = FormatDescriptor.cAlphaBits;
		break;
	//Buffer
	case EGL_BUFFER_SIZE:
		(*value) = FormatDescriptor.cColorBits;
		break;
	//MSAA
	case EGL_SAMPLE_BUFFERS:
		(*value) = 0;
		break;
	case EGL_SAMPLES:
		(*value) = 0;
		break;
	//DepthStencil
	case EGL_DEPTH_SIZE:
		(*value) = FormatDescriptor.cDepthBits;
		break;
	case EGL_STENCIL_SIZE:
		(*value) = FormatDescriptor.cStencilBits;
		break;
	//AlphaMask
	case EGL_ALPHA_MASK_SIZE:
		(*value) = 0;
		break;
	//WINDOW / PIXMAP / PBUFFER
	case EGL_SURFACE_TYPE:
		assert(FormatDescriptor.dwFlags&PFD_DRAW_TO_WINDOW);
		(*value) = EGL_WINDOW_BIT;
		break;
	//OpenGL ES
	case EGL_RENDERABLE_TYPE:
	case EGL_CONFORMANT:
		assert(FormatDescriptor.dwFlags&PFD_SUPPORT_OPENGL);
		(*value) = EGL_OPENGL_ES2_BIT | EGL_OPENGL_ES3_BIT_KHR;
		break;
	//DoubleBuffer
	case EGL_MAX_SWAP_INTERVAL:
		assert(FormatDescriptor.dwFlags&PFD_DOUBLEBUFFER);
		(*value) = 2;
		break;
	case EGL_MIN_SWAP_INTERVAL:
		assert(FormatDescriptor.dwFlags&PFD_DOUBLEBUFFER);
		(*value) = 0;
		break;
	//Transparent
	case EGL_TRANSPARENT_TYPE:
		(*value) = EGL_NONE;
		break;
	case EGL_TRANSPARENT_RED_VALUE:
	case EGL_TRANSPARENT_GREEN_VALUE:
	case EGL_TRANSPARENT_BLUE_VALUE:
		break;
	//GDI
	case EGL_NATIVE_RENDERABLE:
		(*value) = EGL_FALSE;
		break;
	case EGL_NATIVE_VISUAL_ID:
		(*value) = 0;
		break;
	case EGL_NATIVE_VISUAL_TYPE:
		(*value) = EGL_NONE;
		break;
	//
	case EGL_LEVEL:
		(*value) = 0;
		break;
	//
	case EGL_BIND_TO_TEXTURE_RGB:
	case EGL_BIND_TO_TEXTURE_RGBA:
		(*value) = EGL_FALSE;
		break;
	//
	case EGL_MAX_PBUFFER_WIDTH:
	case EGL_MAX_PBUFFER_HEIGHT:
	case EGL_MAX_PBUFFER_PIXELS:
		(*value) = 0;
		break;
	//default:
		//return EGL_FALSE;
	}

	return EGL_TRUE;
}

extern "C" EGLSurface EGLAPIENTRY eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint *attrib_list)
{
	//dpy
	HDC hDC = static_cast<EGLNativeDisplayType>(dpy);

	//config
	int PixelFormatIndex = static_cast<int>(reinterpret_cast<intptr_t>(config));
	
	//win
	HWND hWnd = ::WindowFromDC(hDC);
	assert(hWnd == win);

	if (::GetPixelFormat(hDC) != PixelFormatIndex)
	{
		PIXELFORMATDESCRIPTOR FormatDescriptor;
		int iResult = ::DescribePixelFormat(hDC, PixelFormatIndex, sizeof(FormatDescriptor), &FormatDescriptor);//Load OpenGL ICD
		assert(iResult != 0);

		BOOL wbResult = ::SetPixelFormat(hDC, PixelFormatIndex, &FormatDescriptor);
		assert(wbResult != FALSE);
	}

	return static_cast<EGLSurface>(hDC);
}

extern "C" EGLContext EGLAPIENTRY eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list)
{
	BOOL wbResult;

	//dpy
	HDC hDC = static_cast<EGLNativeDisplayType>(dpy);
	
	//config
	int PixelFormatIndex = static_cast<int>(reinterpret_cast<intptr_t>(config));

	if (::GetPixelFormat(hDC) != PixelFormatIndex)
	{
		PIXELFORMATDESCRIPTOR FormatDescriptor;
		int iResult = ::DescribePixelFormat(hDC, PixelFormatIndex, sizeof(FormatDescriptor), &FormatDescriptor);//Load OpenGL ICD
		assert(iResult != 0);

		wbResult = ::SetPixelFormat(hDC, PixelFormatIndex, &FormatDescriptor);
		assert(wbResult != FALSE);
	}

	//share_context
	HGLRC hShareContext = static_cast<HGLRC>(share_context);

	//attrib_list
	int MajorVersion = 2;
	int MinorVersion = 0;
	int Flags = 0;

	if (attrib_list != NULL)
	{
		std::map<EGLint, EGLint> l_AttribMap;
		for (const EGLint *pAttrib = attrib_list; pAttrib[0] != EGL_NONE; pAttrib += 2)
		{
			l_AttribMap[pAttrib[0]] = pAttrib[1];
		}

		for (std::map<EGLint, EGLint>::const_reference rAttrib : l_AttribMap)
		{
			switch (rAttrib.first)
			{
			case EGL_CONTEXT_MAJOR_VERSION_KHR:
				MajorVersion = rAttrib.second;
				break;
			case EGL_CONTEXT_MINOR_VERSION_KHR:
				MinorVersion = rAttrib.second;
				break;
			case EGL_CONTEXT_FLAGS_KHR:
				if (rAttrib.second&EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR)
				{
					Flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
				}
				break;
			//default:
				//EGL_BAD_ATTRIBUTE
			}
		}
	}

#ifdef ESPROFILE
	int attribList[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB,MajorVersion,
		WGL_CONTEXT_MINOR_VERSION_ARB,MinorVersion,
		WGL_CONTEXT_PROFILE_MASK_ARB,WGL_CONTEXT_ES_PROFILE_BIT_EXT,//OpenGL ES
		WGL_CONTEXT_FLAGS_ARB ,Flags,
		0
	};
#else
	int attribList[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB,4,
		WGL_CONTEXT_MINOR_VERSION_ARB,5,
		WGL_CONTEXT_PROFILE_MASK_ARB,WGL_CONTEXT_CORE_PROFILE_BIT_ARB,//RenderDoc不支持OpenGL ES
		WGL_CONTEXT_FLAGS_ARB ,Flags,
		0
	};
#endif

	//ctx
	HGLRC hContext = s_pFn_wglCreateContextAttribsARB(hDC, hShareContext, attribList);
	return static_cast<EGLContext>(hContext);
}

extern "C" EGLBoolean EGLAPIENTRY eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
	HDC hDrawDC = static_cast<HDC>(draw);
	HDC hReadDC = static_cast<HDC>(read);
	HGLRC hglrc = static_cast<HGLRC>(ctx);

#ifdef ESPROFILE
	return s_pFn_wglMakeContextCurrentARB(hDrawDC, hReadDC, hglrc);
#else
	//RenderDoc不支持wglMakeContextCurrentARB
	assert(hDrawDC == hReadDC);
	return ::wglMakeCurrent(hDrawDC, hglrc);
#endif
}

extern "C" EGLBoolean EGLAPIENTRY eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
	return s_pFn_wglSwapIntervalEXT(interval) == TRUE ? EGL_TRUE : EGL_FALSE;
}

extern "C" EGLBoolean EGLAPIENTRY eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
	HDC hDC = static_cast<HDC>(surface);
	return ::SwapBuffers(hDC) == TRUE ? EGL_TRUE : EGL_FALSE;
}

extern "C"  __eglMustCastToProperFunctionPointerType EGLAPIENTRY eglGetProcAddress(const char *procname)
{
	HMODULE hDlllibEGL = ::GetModuleHandleW(L"libEGL");
	assert(hDlllibEGL != NULL);
	
	return reinterpret_cast<__eglMustCastToProperFunctionPointerType>(::GetProcAddress(hDlllibEGL, procname));
}

extern "C" void GL_APIENTRY glDebugMessageControlKHR(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled)
{
	return s_pFn_glDebugMessageControlKHR(source, type, severity, count, ids, enabled);
}
extern "C" void GL_APIENTRY glDebugMessageCallbackKHR(GLDEBUGPROCKHR callback, const void *userParam)
{
	return s_pFn_glDebugMessageCallbackKHR(callback, userParam);
}

extern "C" void APIENTRY glGenBuffers(GLsizei n, GLuint *buffers)
{
	return s_pFn_glGenBuffers(n, buffers);
}
extern "C" void APIENTRY glGenTextures(GLsizei n, GLuint *textures)
{
	return s_pFn_glGenTextures(n, textures);
}

extern "C" void APIENTRY glBindBuffer(GLenum target, GLuint buffer)
{
	return s_pFn_glBindBuffer(target, buffer);
}
extern "C" void APIENTRY glActiveTexture(GLenum texture)
{
	return s_pFn_glActiveTexture(texture);
}
extern "C" void APIENTRY glBindTexture(GLenum target, GLuint texture)
{
	return s_pFn_glBindTexture(target, texture);
}

extern "C" void GL_APIENTRY glBufferData(GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{
	return s_pFn_glBufferData(target, size, data, usage);
}

extern "C" void GL_APIENTRY glTexBufferOES(GLenum target, GLenum internalformat, GLuint buffer)
{
	return s_pFn_glTexBufferOES(target, internalformat, buffer);
}

extern "C" void APIENTRY glTexStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{
	return s_pFn_glTexStorage2D(target, levels, internalformat, width, height);
}
extern "C" void APIENTRY glTexStorage2DMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations)
{
	return s_pFn_glTexStorage2DMultisample(target, samples, internalformat, width, height, fixedsamplelocations);
}

extern "C" void GL_APIENTRY glTexStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{
	return s_pFn_glTexStorage3D(target, levels, internalformat, width, height, depth);
}

extern "C" void GL_APIENTRY glCompressedTexImage3DOES(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data)
{
	return s_pFn_glCompressedTexImage3DOES(target, level, internalformat, width, height, depth, border, imageSize, data);
}

extern "C" void APIENTRY glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void *data)
{
	return s_pFn_glBufferSubData(target, offset, size, data);
}
extern "C" void APIENTRY glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
{
	return s_pFn_glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

extern "C" void GL_APIENTRY glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels)
{
	return s_pFn_glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
}

extern "C" void APIENTRY glGenSamplers(GLsizei count, GLuint *samplers)
{

	return s_pFn_glGenSamplers(count, samplers);
}
extern "C" void APIENTRY glSamplerParameteri(GLuint sampler, GLenum pname, GLint param)
{
	return s_pFn_glSamplerParameteri(sampler, pname, param);
}
extern "C" void APIENTRY glSamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat *param)
{
	return s_pFn_glSamplerParameterfv(sampler, pname, param);
}

extern "C" void APIENTRY glGenFramebuffers(GLsizei n, GLuint *framebuffers)
{
	return s_pFn_glGenFramebuffers(n, framebuffers);
}
extern "C" void APIENTRY glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	return s_pFn_glBindFramebuffer(target, framebuffer);
}
extern "C" void APIENTRY glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	return s_pFn_glFramebufferTexture2D(target, attachment, textarget, texture, level);
}
extern "C" void GL_APIENTRY glReadBuffer(GLenum src)
{
	return s_pFn_glReadBuffer(src);
}
extern "C" void APIENTRY glDrawBuffers(GLsizei n, const GLenum *bufs)
{
	return s_pFn_glDrawBuffers(n, bufs);
}
extern "C" GLenum APIENTRY glCheckFramebufferStatus(GLenum target)
{
	return s_pFn_glCheckFramebufferStatus(target);
}

extern "C" GLuint APIENTRY glCreateShader(GLenum type)
{
	return s_pFn_glCreateShader(type);
}
extern "C" void APIENTRY glShaderSource(GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length)
{
	return s_pFn_glShaderSource(shader, count, string, length);
}
extern "C" void APIENTRY glCompileShader(GLuint shader)
{
	return s_pFn_glCompileShader(shader);
}
extern "C" void APIENTRY glGetShaderiv(GLuint shader, GLenum pname, GLint *params)
{
	return s_pFn_glGetShaderiv(shader, pname, params);
}
extern "C" void APIENTRY glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
	return s_pFn_glGetShaderInfoLog(shader, bufSize, length, infoLog);
}

extern "C" GLuint APIENTRY glCreateProgram(void)
{
	return s_pFn_glCreateProgram();
}
extern "C" void APIENTRY glAttachShader(GLuint program, GLuint shader)
{
	return s_pFn_glAttachShader(program, shader);
}
extern "C" void APIENTRY glLinkProgram(GLuint program)
{
	return s_pFn_glLinkProgram(program);
}
extern "C" void APIENTRY glGetProgramiv(GLuint program, GLenum pname, GLint *params)
{
	return s_pFn_glGetProgramiv(program, pname, params);
}
extern "C" void APIENTRY glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)
{
	return s_pFn_glGetProgramInfoLog(program, bufSize, length, infoLog);
}

extern "C" void APIENTRY glGenVertexArrays(GLsizei n, GLuint *arrays)
{
	return s_pFn_glGenVertexArrays(n, arrays);
}
extern "C" void APIENTRY glBindVertexArray(GLuint array)
{
	return s_pFn_glBindVertexArray(array);
}
extern "C" void APIENTRY glVertexAttribFormat(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset)
{
	s_pFn_glVertexAttribFormat(attribindex, size, type, normalized, relativeoffset);
}
extern "C" void APIENTRY glEnableVertexAttribArray(GLuint index)
{
	return s_pFn_glEnableVertexAttribArray(index);
}
extern "C" void APIENTRY glVertexAttribBinding(GLuint attribindex, GLuint bindingindex)
{
	return s_pFn_glVertexAttribBinding(attribindex, bindingindex);
}

extern "C" void APIENTRY glEnable(GLenum cap)
{
	return s_pFn_glEnable(cap);
}
extern "C" void APIENTRY glDisable(GLenum cap)
{
	return s_pFn_glDisable(cap);
}
extern "C" void APIENTRY glUseProgram(GLuint program)
{
	return s_pFn_glUseProgram(program);
}
extern "C" void APIENTRY glBindBufferBase(GLenum target, GLuint index, GLuint buffer)
{
	return s_pFn_glBindBufferBase(target, index, buffer);
}
extern "C" void APIENTRY glBindSampler(GLuint unit, GLuint sampler)
{
	return s_pFn_glBindSampler(unit, sampler);
}
extern "C" void GL_APIENTRY glBindImageTexture(GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)
{
	return s_pFn_glBindImageTexture(unit, texture, level, layered, layer, access, format);
}

extern "C" void APIENTRY glClipControlEXT(GLenum origin, GLenum depth)
{
	return s_pFn_glClipControlEXT(origin, depth);
}
extern "C" void APIENTRY glViewportArrayvOES(GLuint first, GLsizei count, const GLfloat *v)
{
	return s_pFn_glViewportArrayvOES(first, count, v);
}
extern "C" void APIENTRY glPolygonOffsetClampEXT(GLfloat factor, GLfloat units, GLfloat clamp)
{
	return s_pFn_glPolygonOffsetClampEXT(factor, units, clamp);
}

extern "C" void GL_APIENTRY glPolygonModeNV(GLenum face, GLenum mode)
{
	return s_pFn_glPolygonModeNV(face, mode);
}

extern "C" void GL_APIENTRY glFrontFace(GLenum mode)
{
	return s_pFn_glFrontFace(mode);
}
extern "C" void GL_APIENTRY glCullFace(GLenum mode)
{
	return s_pFn_glCullFace(mode);
}

extern "C" void APIENTRY glBindVertexBuffer(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride)
{
	return s_pFn_glBindVertexBuffer(bindingindex, buffer, offset, stride);
}
extern "C" void GL_APIENTRY glPatchParameteriOES(GLenum pname, GLint value)
{
	return s_pFn_glPatchParameteriOES(pname, value);
}
extern "C" void APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	return s_pFn_glDrawArrays(mode, first, count);
}
extern "C" void APIENTRY glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
{
	return s_pFn_glDrawElements(mode, count, type, indices);
}

extern "C" void GL_APIENTRY glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{
	return s_pFn_glDrawElementsInstanced(mode, count, type, indices, instancecount);
}

extern "C" void GL_APIENTRY glDepthFunc(GLenum func)
{
	return s_pFn_glDepthFunc(func);
}

extern "C" void GL_APIENTRY glBlendFuncSeparateiOES(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
	return s_pFn_glBlendFuncSeparateiOES(buf, srcRGB, dstRGB, srcAlpha, dstAlpha);
}

extern "C" void GL_APIENTRY glBlendEquationSeparateiOES(GLuint buf, GLenum modeRGB, GLenum modeAlpha)
{
	return s_pFn_glBlendEquationSeparateiOES(buf, modeRGB, modeAlpha);
}

extern "C" void APIENTRY glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value)
{
	return s_pFn_glClearBufferfv(buffer, drawbuffer, value);
}

extern "C" void APIENTRY glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
	return s_pFn_glBlitFramebuffer(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

extern "C" void APIENTRY glDispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z)
{
	return s_pFn_glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
}

extern "C" void GL_APIENTRY glCopyImageSubDataOES(
	GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, 
	GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, 
	GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth
)
{
	return s_pFn_glCopyImageSubDataOES(
		srcName, srcTarget, srcLevel, srcX, srcY, srcZ,
		dstName, dstTarget, dstLevel, dstX, dstY, dstZ,
		srcWidth, srcHeight, srcDepth);
}
