#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "PTTopLevelForm.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int nCmdShow)
{
	PTSceneEditor::PTTopLevelForm formToplevel;
	formToplevel.ShowDialog();
	//formToplevel.Handle��Ϊ�����д���Runtime
	return 0;
}
