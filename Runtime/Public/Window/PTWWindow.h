#ifndef PT_WINDOW_WINDOW_H
#define PT_WINDOW_WINDOW_H

#include "../PTCommon.h"
#include "PTWCommon.h"

#if defined(PTWIN32)
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#if defined(PTWIN32DESKTOP)
typedef HINSTANCE PTWHDisplay;
typedef HWND PTWHWindow;
#elif defined(PTWIN32RUNTIME)
#include <Unknwn.h>
typedef void *PTWHDisplay;
typedef IUnknown *PTWHWindow;
#else
#error 未知的平台
#endif
#elif defined(PTPOSIX)
#if defined(PTPOSIXXCB)
#include <xcb/xcb.h>
typedef xcb_connection_t *PTWHDisplay;
typedef xcb_window_t PTWHWindow;
#elif defined(PTPOSIXANDROID)
#include <android/native_window.h>
typedef void *PTWHDisplay;
typedef ANativeWindow *PTWHWindow;
#else
#error 未知的平台
#endif
#else
#error 未知的平台
#endif

#include <stdint.h>

struct IPTWWindow
{
	//Output
	virtual void EventOutputCallback_Hook(void *pUserData, void(PTPTR *pEventOutputCallback)(void *pUserData, void *pOutputData)) = 0;
	//再PTGDevice中需要维护一份Current_XXX，比较以确定是否Changed
	//一般传入PTGDevice中的方法，用于构造交换律缓冲

	//在OutputEventCallback_Hook为空时会忙式等待 由于系统导致Window模块比Application模块前启动（而非在ApplicationMain中由应用层创建，因为必须等待应用层以同步）

	struct EventOutput
	{
		enum : uint32_t
		{
			Type_Unknown = 0,
			Type_WindowCreated = 1,
			Type_WindowResized = 2,
			Type_WindowDestroyed = 3,
		}m_Type;
	};

	struct EventOutput_WindowCreated : EventOutput
	{
		PTWHDisplay m_hDisplay;
		PTWHWindow m_hWindow;
		uint32_t m_Width;
		uint32_t m_Height;
	};
	
	struct EventOutput_WindowResized : EventOutput
	{
		PTWHDisplay m_hDisplay;
		PTWHWindow m_hWindow;
		uint32_t m_Width;
		uint32_t m_Height;
	};
	
	struct EventOutput_WindowDestroyed : EventOutput
	{

	};

	//Input
	//MessagePump线程
	virtual void EventInputCallback_Hook(void *pUserData, void(PTPTR *pEventInputCallback)(void *pUserData, void *pInputData)) = 0;
	//KeyDown
	static const uint32_t InputType_KeyDown = 0;
	static const uint32_t KeyType_A = 0;
	static const uint32_t KeyType_B = 1;
	static const uint32_t KeyType_C = 2;
	static const uint32_t KeyType_D = 3;
	static const uint32_t KeyType_E = 4;
	static const uint32_t KeyType_F = 5;
	static const uint32_t KeyType_G = 6;
	static const uint32_t KeyType_H = 7;
	static const uint32_t KeyType_I = 8;
	static const uint32_t KeyType_J = 9;
	static const uint32_t KeyType_K = 10;
	static const uint32_t KeyType_L = 11;
	static const uint32_t KeyType_M = 12;
	static const uint32_t KeyType_N = 13;
	static const uint32_t KeyType_O = 14;
	static const uint32_t KeyType_P = 15;
	static const uint32_t KeyType_Q = 16;
	static const uint32_t KeyType_R = 17;
	static const uint32_t KeyType_S = 18;
	static const uint32_t KeyType_T = 19;
	static const uint32_t KeyType_U = 20;
	static const uint32_t KeyType_V = 21;
	static const uint32_t KeyType_W = 22;
	static const uint32_t KeyType_X = 23;
	static const uint32_t KeyType_Y = 24;
	static const uint32_t KeyType_Z = 25;
	static const uint32_t KeyType_0 = 26;
	static const uint32_t KeyType_1 = 27;
	static const uint32_t KeyType_2 = 28;
	static const uint32_t KeyType_3 = 29;
	static const uint32_t KeyType_4 = 30;
	static const uint32_t KeyType_5 = 31;
	static const uint32_t KeyType_6 = 32;
	static const uint32_t KeyType_7 = 33;
	static const uint32_t KeyType_8 = 34;
	static const uint32_t KeyType_9 = 35;
	static const uint32_t KeyType_F1 = 36;
	static const uint32_t KeyType_F2 = 37;
	static const uint32_t KeyType_F3 = 38;
	static const uint32_t KeyType_F4 = 39;
	static const uint32_t KeyType_F5 = 40;
	static const uint32_t KeyType_F6 = 41;
	static const uint32_t KeyType_F7 = 42;
	static const uint32_t KeyType_F8 = 43;
	static const uint32_t KeyType_F9 = 44;
	static const uint32_t KeyType_F10 = 45;
	static const uint32_t KeyType_F11 = 46;
	static const uint32_t KeyType_F12 = 47;
	static const uint32_t KeyType_Space = 48;
	static const uint32_t KeyType_Unknown = 100;
	//KeyUp
	static const uint32_t InputType_KeyUp = 1;
	//KeyType
	//PointButtonPress
	static const uint32_t InputType_PointButtonPress = 2;
	//float Position_X
	//float Position_Y
	static const uint32_t PointButtonType_Left = 0;
	static const uint32_t PointButtonType_Right = 1;
	static const uint32_t PointButtonType_Middle = 2;
	static const uint32_t PointButtonType_Unknown = 100;
	//uint32_t PointId
	//PointButtonRelease
	static const uint32_t InputType_PointButtonRelease = 3;
	//float Position_X
	//float Position_Y
	//uint32_t PointButtonType
	//uint32_t PointId
	//PointMove
	static const uint32_t InputType_PointMove = 4;
	//float Position_X
	//float Position_Y
	//uint32_t PointId

	static const uint32_t InputType_GameController_ButtonDown = 5;
	//uint32_t ButtonType
	static const uint32_t GameController_ButtonType_X = 0;
	static const uint32_t GameController_ButtonType_Y = 1;
	static const uint32_t GameController_ButtonType_A = 2;
	static const uint32_t GameController_ButtonType_B = 3;
	//bool IsButtonXPressed //消息发出时的状态 //API(joyGetPos(Ex))获取的是当前的状态
	//bool IsButtonYPressed
	//bool IsButtonAPressed
	//bool IsButtonBPressed

	static const uint32_t InputType_GameController_ButtonUp = 6;

	//Authority
	virtual void Parent_Set(PTWHWindow hWindowParent) = 0;
	virtual void Position_Set(uint32_t TopLeftX, uint32_t TopLeftY) = 0;
	virtual void Size_Set(uint32_t Width, uint32_t Height) = 0;
};

#endif
