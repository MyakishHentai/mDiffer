#ifndef H_Projection
#define H_Projection

#include "mDiffer.h"

// ����� ������������� �� �����.
class Projection
{
public:
	Projection();
	~Projection();

	VOID SetPath(wstring x);						//����� ���������� �����.
	VOID SetBytesInBlock(__in DWORD x);				//������ ���-�� ������������ ������ � 1 �����.
	BOOL CreateMapping();							//�������� ��������.
	BOOL CreateMapView(__in long long fileOffSet);	//�������� �������������.
	VOID UnmapView();								//������������ �������������.
	VOID CloseMap();								//�������� ��������.
	wstring GetPath();								//���������� ���������� �����.
	DWORD GetBytesInBlock();						//���������� ���-�� ������������ ������ � 1 �����.
	long long GetFileSizing();						//���������� ������ �����.
	BYTE GetProjFile(__in long long index);			//���������� ������������� �� �����.
	string GetState();								//���������� ���������.
	VOID StartWork();								//������ ������.

private:
	string m_State;									//��������� �������.
	wstring m_Path;									//����� �����.	
	HANDLE m_FileMapping;							//������� �����.
	DWORD m_BytesInBlock;							//���������� ������������ ������ � 1 �����.
	long long m_FileSizing;							//������ �����.
	PBYTE m_ProjFile;								//������������� � �����.
	bool m_StartWork;								//���� ������ ������.
};

#endif