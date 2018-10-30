#include <windows.h>
#include "SetForm.h"

using namespace System;
using namespace System::Windows::Forms;
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE HpREViNSTANCE,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	Application::Run(gcnew SetForm());
	return 0;
}