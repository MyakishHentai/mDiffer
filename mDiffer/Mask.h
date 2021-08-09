#ifndef H_MASK
#define H_MASK

#include "ChildWin.h"

// ����� ��� �������� ����� ����� �� ������.
class Mask
{
public:
	//static bool checkIdent;											// �������� �� ������������ ������.
	static VOID SetLineOfFileMax(__in vector<ChildWin*> Child);			// ������ ���������� ����� � ������.
	static VOID SetVScrollPos(__in long long vscrollPos);				// ������ ������� ������������ ������ �������.
	static long long GetLineOfFileMax();								// �������� ���������� �������� ����� � �����.	
	static long long GetVscrollPos();									// �������� ������� ������������ ������ �������.
	//bool checkIdent;													// �������� �� ������������ ������.
	static VOID CreateMask(__in vector<ChildWin*> Child);				// ��������� ����������� � �����������.
	static vector<bool>& GetMask();										// ������� ��������������� �����.

private:
	static SYSTEM_INFO m_SystemInfo;									// ��������� ����������.
	static::vector<bool> m_Mask;										// ������, �������� ��� �������� (false - �� ���������, true - ���������).
	static long long m_PaintBeg;										// ����� ������ ������ ���������.
	static long long m_PaintEnd;										// ����� ������ ����� ���������.
	static long long m_LineOfFileMax;									// ���������� �������� ����� � �����.	
	static long long m_VscrollPos;										// ������� ������������ ������ �������.
	static long long qwFileSizeMax;										// ������������ ������ �����.
};
#endif


