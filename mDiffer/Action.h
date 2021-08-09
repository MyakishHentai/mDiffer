#include "framework.h"
#include "Mask.h"
#include "ChildWin.h"

#ifndef ACTION_H
#define ACTION_H

class Action
{
private:
	static Mask m_Mask;
	static long long m_ChildVScrollMax;												// ������������ ������� ������������ ������ �������.
	static float m_ChildCoeffVScroll;												// ����������� ��������� ���-�� ������-������� � ������������� �������� � ��������� = INT32_MAX.
	static int cxClient;															// ������ ����������� ����.
	static int cyClient;															// ������ ����������� ����.

public:
	// ���������� �������� � �������� �����.
	static LRESULT CALLBACK					WndProc(__in HWND m_hWnd, __in  UINT message, __in  WPARAM wParam, __in  LPARAM lParam);
	// ��������� ������.
	static int								WndCommand(__in WPARAM wParam);
	// ���� � ������� �����������.
	static INT_PTR CALLBACK					About(__in HWND hDlg, __in  UINT message, __in  WPARAM wParam, __in  LPARAM lParam);
	static BOOL CALLBACK					DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	// �������� �� ���������� ��������� �������� ��� �� ����������� ������������� �����������.
	static int								CorrectChoice(__in int sel_num_Win);
	// ������� ������� ������ ChildWin.
	static void							    AddControls(__in HWND m_hWnd, __in int count_N);
	// "����� �����" - ��������� �������� ���� � ������� ����, ���� � �.�..
	static int								Start(__in HINSTANCE hInstance, __in  HINSTANCE hPrevInstance, __in  PSTR szCmdLine, __in  int nCmdShow);
	// ������ ����� � ���������� ����������.
	static int								ChoiceFile(__in WPARAM wParam);		
	// ��������� �������� � �������� �����.
	static LRESULT CALLBACK					ChildProc(__in HWND hwnd, __in UINT iMsg, __in WPARAM wParam, __in LPARAM lParam);
	// ��������� ������� ����� ��� ������.
	static int								ChildSetVScroll();
	// ������ �� ��������� � �������� �����.
	static int								ChildVScroll(__in HWND hwnd, __in WPARAM wParam);	
	// ������ ��������� ����.
	static int								ChildMouseWheel(__in WPARAM wParam);	
	// ��������� ������� �� �������.
	static int								KeyboardEvents(__in WPARAM wParam);	
	// �������� ���� � ����������� �� ������� ��������, ���������������.
	static VOID								MoveWindows(__in int sbPos);											
	// ��������� ��������� �����������.
	static VOID							    Change(__in bool fileWasChoice);	
	// ��������� ������������ ������ �� ��������.
	static VOID								VscrollProc(__inout long long vScrollInc);		
	// ��������� ��������� ������.
	static VOID								CheckState();														
	// ���������� ����������:											
	
	static HINSTANCE hInstance;														// ���������� ������.
	static HWND m_hWnd;																// ���������� ����.
};

#endif

