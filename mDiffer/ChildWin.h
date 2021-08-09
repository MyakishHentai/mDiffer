#include "framework.h"
#include "Projection.h"
#ifndef CHILDWIN_H
#define CHILDWIN_H

// ����� ��� �������� ��������� �������� ���� - ������(����� �����), ���� Edit(����������), ��������� ����(����� �����).
class ChildWin
{
private:
	// ������ ����������� ��� �������� ����.
	static HWND m_hWnd;
	static HINSTANCE hInstance;
	static int count_N;
	int maxPow;
	// ��������������� ������ �������� � �������� ����.
	void CreateBox(int number_Obj);
	void Destroy();
	// ����������� ����.											
	HWND m_Edit;
	HWND m_Button;
	HWND m_WndChild;
	Projection m_Projection;																			// ����.	
	SYSTEM_INFO m_SystemInfo;																			// ��������� ����������.
	long long m_VscrollPos;																				// ������� ������������ ������ �������.
	long long m_PaintBeg;																				// ����� ������ ������ ���������.
	long long m_PaintEnd;																				// ����� ������ ����� ���������.
	vector<char> Element;																				// ������ ��������� �� �����. 

public:
	VOID Set_maxPow(int value);																			// ��������� �����������.
	VOID Print(__in const vector<bool>& m_Mask, long long scroll);										// ��������� ����������� � �����������.
	VOID Push_Element(char elem);																		// ���������� �������� �� ����� � �������� ����.
	VOID Clear(BOOL value);																				// ������� ����.
	static VOID PrintNumber(__in long long pos, __in int maxPow, __in HDC hdc, __in int x, __in int y);	// ����� ������ �������� �� ��� ����.
	static VOID PrintBinary(__in char sign, __in HDC hdc, __in int x, __in int y);						// ����� �������� � �������� ����.
	static VOID PrintText(__in char elem, __in HDC hdc, __in int x, __in int y);						// ����� ��������.
	Projection* GetProjection();																		// ��������� ��������� �� �������������.	
	VOID Set_m_PaintEnd(long long End);																	// ��������� ������ �����
	VOID Set_m_PaintBeg(long long Begin);																// �� ������ Mask.
	ChildWin();																							// ������������.
	ChildWin(int number_Obj);
	~ChildWin();																						// ����������.
	// Get\Set ��� ������������ � ���������� ��������.
	HWND Get_m_WndChild();
	HWND Get_m_Edit();
	HWND Get_m_Button();
	static void Set_hWnd(HWND hWnd0);
	static void Set_N(__in int Count_N);
	static void Set_hInst(__in HINSTANCE hInst);
};

#endif

