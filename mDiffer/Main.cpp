#include "Action.h"
#include "framework.h"
//Точка входа в программу.
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	Action::Start(hInstance, hPrevInstance, szCmdLine, nCmdShow);
	return 0;
}


