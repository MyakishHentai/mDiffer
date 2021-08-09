#include "ChildWin.h"
#include "mDiffer.h"
#include "Mask.h"
HWND ChildWin::m_hWnd;
HINSTANCE ChildWin::hInstance;
int ChildWin::count_N;
int x_but_edit = 10;

// ���������� �����������.
VOID ChildWin::Set_maxPow(int value) {
	maxPow = value;
}

// �������� ������� ��� ������.
VOID ChildWin::Push_Element(char elem) {
	Element.push_back(elem);
}

// ������� ����� � ����.
VOID ChildWin::Print(__in const vector<bool>& m_Mask, long long scroll)
{
	GetSystemInfo(&m_SystemInfo);
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = BeginPaint(Get_m_WndChild(), &ps);
	/*PAINTSTRUCT pst;
	RECT rcClientRect;
	GetClientRect(Get_m_WndChild(), &rcClientRect);
	HDC memDC = CreateCompatibleDC(hdc);
	HBITMAP bmp = CreateCompatibleBitmap(hdc,
		rcClientRect.right - rcClientRect.left,
		rcClientRect.bottom - rcClientRect.top);
	HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, bmp);*/

	//���������� ������� �������� �����
	auto pos = m_PaintBeg * SIZING_LINE;
	m_VscrollPos = scroll;
	//������ ��� ��������� ������

	const auto reservNumberX = CX_CHAR * (maxPow + SPACE_STANDARD);
	//������ ��� ��������� ������
	const auto reservBinarX = CX_CHAR * (SIZING_LINE * (LENGTH_BINARY + BINARY_BARRIER));


	//���������� �� X
	const auto x = CX_CHAR;
	//���������� �� Y
	auto y = static_cast<int>(CY_CHAR * (1 - m_VscrollPos + m_PaintBeg));

	//������� ������������� �������� ��������
	if (m_PaintBeg < m_PaintEnd)
	{
		PrintNumber(pos, maxPow, hdc, x, y);
	}


	//������� ����� � �������� ������
	auto j = 0;
	int l = 0;
	for (auto i = m_PaintBeg; i < m_PaintEnd; j++, pos++, l++)
	{

		//������� �� ����� ������
		if (j == SIZING_LINE)
		{
			j = 0;
			i++;
			y += CY_CHAR;
			if (i < m_PaintEnd)
			{
				PrintNumber(pos, maxPow, hdc, x, y);
			}
		}
		//������� �� ���������� �����������
		if (l < Element.size())
		{
			auto color = m_Mask[l];

			if (color)
			{
				SetTextColor(hdc, TRUE_TEXT_COLOR);
			}
			else
			{
				SetTextColor(hdc, FALSE_TEXT_COLOR);
			}
			//�����
			PrintBinary(Element[l], hdc, x + reservNumberX + (j * LENGTH_BINARY * CX_CAPS), y);
			PrintText(Element[l], hdc, x + reservNumberX + reservBinarX + (j * CX_CAPS), y);
		}
	}

	/*BitBlt(hdc, 0, 0, PaintStruct.rcClientRect.right - rcClientRect.left,
		rcClientRect.bottom - rcClientRect.top, memDC, 0, 0, SRCCOPY);
	SelectObject(memDC, oldBmp); 
	DeleteObject(bmp);
	DeleteDC(memDC); */ 

	//���������� ������
	EndPaint(Get_m_WndChild(), &ps);
	//Clear();
	Element.clear();
}

// ��������.
VOID ChildWin::Clear(BOOL value)
{
	RECT rect;
	GetClientRect(Get_m_WndChild(), &rect);
	InvalidateRect(Get_m_WndChild(), &rect, value);
}

// �������� ��������� �� ������������ �����.
Projection* ChildWin::GetProjection()
{
	return &m_Projection;
};

// ����� ������ �������� �� ��� ����.
VOID ChildWin::PrintNumber(__in long long pos, __in int maxPow, __in HDC hdc, __in int x, __in int y)
{
	//��������� ������� ���������� ����������� �����
	auto help_int = pos;
	//16-�� �������
	static const char sixty[16]{ '0','1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	//����� ������� ��������� ������
	const auto numbersPos = new char[maxPow + 2]{};

	for (long long p = maxPow; p >= 0; p--)
	{
		if (help_int != 0)
		{
			numbersPos[p] = (char)(sixty[help_int % DIGITS_SYSTEM]);
		}
		else
		{
			numbersPos[p] = '0';
		}
		help_int /= DIGITS_SYSTEM;
	}


		SetTextColor(hdc, NUMBERS_COLOR);
		TextOut(
			hdc, x, y,
			numbersPos,
			strlen(numbersPos)
		);

	delete[] numbersPos;
}

// ����� �������� � �������� ����.
VOID ChildWin::PrintBinary(__in char sign, __in HDC hdc, __in int x, __in int y)
{
	//16-�� �������
	static const char sixty[16] = { '0','1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	char ch[2];
	ch[0] = sixty[(sign & 0xF0) >> 4];
	ch[1] = sixty[(sign & 0x0F) >> 0];

	char elem[2];
	elem[0] = ch[0];
	elem[1] = NULL;

	TextOut(
		hdc, x, y,
		elem,
		strlen(elem)
	);
	x += CX_CAPS;
	elem[0] = ch[1];
	TextOut(
		hdc, x, y,
		elem,
		strlen(elem)
	);
}

// ����� ��������.
VOID ChildWin::PrintText(__in char elem, __in HDC hdc, __in int x, __in int y)
{
	char out[2]{ elem, NULL };

	if (out[0] == NULL)
	{
		out[0] = '.';
	}
	TextOut(
		hdc, x, y,
		out,
		strlen(out)
	);
};

void ChildWin::CreateBox(int number_Obj)
{
	string edit_text = "������� ���� � �����: " + to_string(number_Obj + 1);
	m_Button = CreateWindow(TEXT("Button"), TEXT(">>"),
		WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
		x_but_edit, 10, 
		BUTTON_WIDTH, BUTTON_HEIGHT, 
		m_hWnd, reinterpret_cast<HMENU>(static_cast<const int>(BUTTON_FIRST + number_Obj)), nullptr, nullptr);
	m_Edit = CreateWindow(TEXT("Edit"), TEXT((edit_text).c_str()),
		WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_READONLY | WS_BORDER | ES_AUTOVSCROLL,
		x_but_edit + 50, 10, 
		EDIT_WIDTH, EDIT_HEIGHT, 
		m_hWnd, reinterpret_cast<HMENU>(static_cast<const int>(EDIT_FIRST + number_Obj)), nullptr, nullptr);
	m_WndChild = (CreateWindowEx(
		0,
		"Child",
		static_cast<LPCSTR>(NULL),
		WS_CHILD | WS_BORDER | WS_VISIBLE | WS_VSCROLL,
		x_but_edit, 60,
		CHILD_WIDTH, CHILD_HEIGHT,
		m_hWnd, reinterpret_cast<HMENU>(static_cast<const int>(CHILD_FIRST + number_Obj)), hInstance, NULL
	));
	EnableScrollBar(m_WndChild, SB_VERT, ESB_DISABLE_BOTH);

	if (!(m_Button && m_Edit && m_WndChild))
	{
		MessageBox(nullptr,
			"�� ������� ������� ����, ������������ ����������!",
			"������", MB_ICONEXCLAMATION | MB_OK);;
	}
}

// ������� �������� ���� ��� ������ �����������.
void ChildWin::Destroy()
{
	DestroyWindow(m_Button);
	DestroyWindow(m_Edit);
	CloseWindow(m_WndChild);
	// ������� "�������" ��������.
	x_but_edit -= 420;
}

// ����������� �� ��������� (������� 2 ����).
ChildWin::ChildWin()
{
	CreateBox(0);
	//Create_DOWN(1);
	x_but_edit += 420;
	m_PaintBeg = 0;
	m_PaintEnd = 0;
	m_VscrollPos = 0;
}

// ������� �������� ���-�� ����.
ChildWin::ChildWin(int number_Obj)
{
	CreateBox(number_Obj);
	x_but_edit += 420;	
	m_PaintBeg = 0;
	m_PaintEnd = 0;
	m_VscrollPos = 0;
}

// ����������.
ChildWin::~ChildWin()
{
	Destroy();
}

// ���������� ����� ������ ������.
VOID ChildWin::Set_m_PaintEnd(long long End)
{
	m_PaintEnd = End;
}

// ���������� ������ ������ ������.
VOID ChildWin::Set_m_PaintBeg(long long Begin)
{
	m_PaintBeg = Begin;
}

// �������� ���������� ��������� ����.
HWND ChildWin::Get_m_WndChild()
{
	return m_WndChild;
}

// �������� ���������� ���� Edit (����������).
HWND ChildWin::Get_m_Edit()
{
	return m_Edit;
}

// �������� ���������� ���� Button (������).
HWND ChildWin::Get_m_Button()
{
	return m_Button;
}

// �������� ���������� ������������� ����.
void ChildWin::Set_hWnd(HWND parrent)
{
	m_hWnd = parrent;
}

// ������ ������� �������� ����.
void ChildWin::Set_N(__in int Count_N)
{
	count_N = Count_N;
}

// ������ ������ ����.
void ChildWin::Set_hInst(__in HINSTANCE hInst)
{
	hInstance = hInst;
}