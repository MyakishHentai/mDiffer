#include "Action.h"
#include "framework.h"
//����� ����� � ���������.
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	Action::Start(hInstance, hPrevInstance, szCmdLine, nCmdShow);
	return 0;
}


