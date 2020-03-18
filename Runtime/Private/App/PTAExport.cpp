#include "../../Public/App/PT_APP_Export.h"

extern "C" PTAPPAPI int PTCALL PTAMain(PT_WSI_IWindow *pWindow, int argc,
									   char *argv[])
{
	pWindow->EventOutputCallback_Hook(
		(void *)1, [](void *pUserData, void *pOutputData) -> void {

		});

	pWindow->EventInputCallback_Hook(
		(void *)1, [](void *pUserData, void *pInputData) -> void {

		});

	return 0;
}

#if 0
#include "../../Public/McRT/PTSMemoryAllocator.h"
#include "../../Public/McRT/PTSTaskScheduler.h"
#include "../../Public/McRT/PTSThread.h"
//#include "../../Public/Video/PTVDInstance.h"
//#include "../../Public/Audio/PTADInstance.h"
#include <assert.h>
#include <iostream>
#include <new>

//#include "../../Public/Math/PTMath.h"

#if defined(PTWIN32)
#if defined(PTWIN32DESKTOP)

#elif defined(PTWIN32RUNTIME)

#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)

#elif defined(PTPOSIXLINUXBIONIC)
#include <dlfcn.h>
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif


extern "C" PTIMPORT void PTCALL PTI_FBXSDK_FBXToPTTF(IPTSFile *pFileFBX, IPTSFile *pPTTF);

#include "../../Public/McRT/PTSMemory.h"

#include "../../Public/GFX/PTGFXHAL.h"
#include "../../Public/GFX/PTGFXHALUtil.h"

extern "C" PTAPPAPI int PTCALL PTAMain(PT_WSI_IWindow *pWindow, int argc, char *argv[])
{

	::PTSMemoryAllocator_Initialize();

	pWindow->EventOutputCallback_Hook(NULL, [](void *pUserData, void *pInputData)->void
	{

	});

	PT_GFX_HAL_IDevice *pDevice = ::PT_GFX_HAL_Device_Init();

	::PT_GFX_HAL_Texture_Asset_LoadDDS(pDevice, "lena_std.dds");

#if 0
	char Src[4096] = { R"(The open philosophy of wiki - allowing anyone to edit content -does not ensure that every editor's intentions are well-mannered. For example, vandalism (changing wiki content to something offensive, adding nonsense, or deliberately adding incorrect information, such as hoax information) can be a major problem. On larger wiki sites, such as those run by the Wikimedia Foundation, vandalism can go unnoticed for some period of time. Wikis, because of their open nature, are susceptible to intentional disruption, known as "trolling". Wikis tend to take a soft-security[28][unreliable source] approach to the problem of vandalism, making damage easy to undo rather than attempting to prevent damage. Larger wikis often employ sophisticated methods, such as bots that automatically identify and revert vandalism and JavaScript enhancements that show characters that have been added in each edit. In this way, vandalism can be limited to just "minor vandalism" or "sneaky vandalism", where the characters added/eliminated are so few that bots do not identify them and users do not pay much attention to them.[29][unreliable source] An example of a bot that reverts vandalism on Wikipedia is ClueBot NG. ClueBot NG can revert edits, often within minutes, if not seconds. The bot uses machine learning in lieu of heuristics.[30] The amount of vandalism a wiki receives depends on how open the wiki is. For instance, some wikis allow unregistered users, identified by their IP addresses, to edit content, while others limit this function to just registered users. Most wikis allow anonymous editing without an account,[31] but give registered users additional editing functions; on most wikis, becoming a registered user is a short and simple process. Some wikis require an additional waiting period before gaining access to certain tools. For example, on the English Wikipedia, registered users can rename pages only if their account is at least four days old and has made at least ten edits. Other wikis such as the Portuguese Wikipedia use an editing requirement instead of a time requirement, granting extra tools after the user has made a certain number of edits to prove their trustworthiness and usefulness as an editor. Vandalism of Wikipedia is common (though policed and usually reverted) because it is extremely open, allowing anyone with a computer and Internet access to edit it, although this makes it grow rapidly. In contrast, Citizendium requires an editor's real name and short autobiography, affecting the growth of the wiki but sometimes helping stop vandalism. Edit wars can also occur as users repetitively revert a page to the version they favor. In some cases, editors with opposing views of which content should appear or what formatting style should be used will change and re-change each other's edits. This results in the page being "unstable" from a general users' perspective, because each time a general user comes to the page, it may look different. Some wiki software allows an administrator to stop such edit wars by locking a page from further editing until a decision has been made on what version of the page would be most appropriate.[9] Some wikis are in a better position than others to control behavior due to governance structures existing outside the wiki. For instance, a college teacher can create incentives for students to behave themselves on a class wiki they administer by limiting editing to logged-in users and pointing out that all contributions can be traced back to the contributors. Bad behavior can then be dealt with in accordance with university policies.[11] The issue of wiki vandalism is debated. In some cases, when an editor deletes an entire article and replaces it with nonsense content, it may be a "test edit", made by the user as she or he is experimenting with the wiki system. Some editors may not realize that they have damaged the page, or if they do realize it, they may not know how to undo the mistake or restore the content.)" };
	char Dst[4096] = { "****************"};

	PTS_MemoryCopy(Dst + 3, Src + 3, 1329);

	//逻辑开发入口点
	pWindow->EventInputCallback_Hook(NULL, [](void *pUserData, void *pInputData)->void
	{

	});

	//PTADInstance_ForProcess();
#endif

#if defined(PTWIN32)
#if defined(PTWIN32DESKTOP)
	//D3D12优先
	//HINSTANCE hLibVideoD3D12 = ::LoadLibraryExW(L"PTVideoD3D12.dll", NULL, 0U);
	//if (hLibVideoD3D12 != NULL)
	//{
	//	decltype(::PTVDInstance_ForProcess) *pFn_PTVDInstance_ForProcess = reinterpret_cast<decltype(::PTVDInstance_ForProcess) *>(::GetProcAddress(hLibVideoD3D12, "PTVDInstance_ForProcess"));
	//	pVDInstance = pFn_PTVDInstance_ForProcess();
	//}

	//尝试Vulkan
	//if (pVDInstance == NULL)
	//{
	//	HINSTANCE hLibVideoVK = ::LoadLibraryExW(L"PTVideoVK.dll", NULL, 0U);
	//	if (hLibVideoVK != NULL)
	//	{
	//		decltype(::PTVDInstance_ForProcess) *pFn_PTVDInstance_ForProcess = reinterpret_cast<decltype(::PTVDInstance_ForProcess) *>(::GetProcAddress(hLibVideoVK, "PTVDInstance_ForProcess"));
	//		IPTVDInstance *pVDInstance = pFn_PTVDInstance_ForProcess();
	//	}
	//}
#elif defined(PTWIN32RUNTIME)

#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXLINUXGLIBC)

#elif defined(PTPOSIXLINUXBIONIC)
	//void *hLibVideoVK = ::dlopen("libPTVideoVK.so", RTLD_NOW | RTLD_LOCAL);
	//if (hLibVideoVK != NULL)
	//{
	//	decltype(::PTVDInstance_ForProcess) *pFn_PTVDInstance_ForProcess = reinterpret_cast<decltype(::PTVDInstance_ForProcess) *>(::dlsym(hLibVideoVK, "PTVDInstance_ForProcess"));
	//	IPTVDInstance *pVDInstance = pFn_PTVDInstance_ForProcess();
	//}
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif

	//pVDInstance->Initialize();

#if 0
	IPTVDSceneTreeNodeCamera *pCamera = NULL;
	IPTVDXBar *pSceneGraph = NULL;
	IPTVDAssertImage2D *pImageOut = NULL;

	//pWindow->EventOutputCallback_Hook(pVDInstance, [](void *pUserData, void *pInputData)->void
	//{
	//	static_cast<IPTVDInstance *>(pUserData)->WindowEventOutputCallback(pInputData);
	//});


	//pVDInstance->Render(pCamera, pCamera, pSceneGraph, pImageOut);
#endif

#if 0
	::PTSMemoryAllocator_Initialize();
	::PTSFileSystem_Initialize();

	IPTSFile *pFileFBX = ::PTSFileSystem_ForProcess()->File_Create(IPTSFileSystem::FILE_OPEN_READONLY, "C:\\Users\\Administrator\\Desktop\\3dsmax.FBX");
#endif
	//PTI_FBXSDK_FBXToPTTF(pFileFBX, NULL);

	return 0;
}

extern "C" PTAPPAPI bool PTCALL PTAFileSystem_Initialize_Hook()
{
	return false;
}

extern "C" PTAPPAPI IPTSFileSystem * PTCALL PTAFileSystem_ForProcess_Hook()
{
	return NULL;
}

#endif