#include "ChildWin.h"
#include "mDiffer.h"
#include "Mask.h"
HWND ChildWin::m_hWnd;
HINSTANCE ChildWin::hInstance;
int ChildWin::count_N;
int x_but_edit = 10;

// Установить разрядность.
VOID ChildWin::Set_maxPow(int value) {
	maxPow = value;
}

// Добавить элемент для вывода.
VOID ChildWin::Push_Element(char elem) {
	Element.push_back(elem);
}

// Вывести текст в окно.
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

	//Абсолютная позиция текущего байта
	auto pos = m_PaintBeg * SIZING_LINE;
	m_VscrollPos = scroll;
	//Отступ для нумерации вывода

	const auto reservNumberX = CX_CHAR * (maxPow + SPACE_STANDARD);
	//Отступ для бинарного вывода
	const auto reservBinarX = CX_CHAR * (SIZING_LINE * (LENGTH_BINARY + BINARY_BARRIER));


	//Координата по X
	const auto x = CX_CHAR;
	//Координата по Y
	auto y = static_cast<int>(CY_CHAR * (1 - m_VscrollPos + m_PaintBeg));

	//Условие необходимости нулевого счетчика
	if (m_PaintBeg < m_PaintEnd)
	{
		PrintNumber(pos, maxPow, hdc, x, y);
	}


	//Позиция байта в выходной строке
	auto j = 0;
	int l = 0;
	for (auto i = m_PaintBeg; i < m_PaintEnd; j++, pos++, l++)
	{

		//Переход на новую строку
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
		//Усливия не совпадения содержимого
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
			//Вывод
			PrintBinary(Element[l], hdc, x + reservNumberX + (j * LENGTH_BINARY * CX_CAPS), y);
			PrintText(Element[l], hdc, x + reservNumberX + reservBinarX + (j * CX_CAPS), y);
		}
	}

	/*BitBlt(hdc, 0, 0, PaintStruct.rcClientRect.right - rcClientRect.left,
		rcClientRect.bottom - rcClientRect.top, memDC, 0, 0, SRCCOPY);
	SelectObject(memDC, oldBmp); 
	DeleteObject(bmp);
	DeleteDC(memDC); */ 

	//Завершение работы
	EndPaint(Get_m_WndChild(), &ps);
	//Clear();
	Element.clear();
}

// Очистить.
VOID ChildWin::Clear(BOOL value)
{
	RECT rect;
	GetClientRect(Get_m_WndChild(), &rect);
	InvalidateRect(Get_m_WndChild(), &rect, value);
}

// Получить указатель на проецироваие файла.
Projection* ChildWin::GetProjection()
{
	return &m_Projection;
};

// Вывод номера элемента во все окна.
VOID ChildWin::PrintNumber(__in long long pos, __in int maxPow, __in HDC hdc, __in int x, __in int y)
{
	//нумерация позиций выводимого содержимого файла
	auto help_int = pos;
	//16-ые символы
	static const char sixty[16]{ '0','1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	//номер позиции элементов вывода
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

// Вывод элемента в бинарном виде.
VOID ChildWin::PrintBinary(__in char sign, __in HDC hdc, __in int x, __in int y)
{
	//16-ые символы
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

// Вывод элемента.
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
	string edit_text = "Укажите путь к файлу: " + to_string(number_Obj + 1);
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
			"Не удалось создать окно, перзапустите приложение!",
			"Ошибка", MB_ICONEXCLAMATION | MB_OK);;
	}
}

// Удалять дочерние окна при вызове дестркутора.
void ChildWin::Destroy()
{
	DestroyWindow(m_Button);
	DestroyWindow(m_Edit);
	CloseWindow(m_WndChild);
	// Возврат "каретки" смещения.
	x_but_edit -= 420;
}

// Конструктор по умолчанию (создать 2 окна).
ChildWin::ChildWin()
{
	CreateBox(0);
	//Create_DOWN(1);
	x_but_edit += 420;
	m_PaintBeg = 0;
	m_PaintEnd = 0;
	m_VscrollPos = 0;
}

// Создать заданное кол-во окон.
ChildWin::ChildWin(int number_Obj)
{
	CreateBox(number_Obj);
	x_but_edit += 420;	
	m_PaintBeg = 0;
	m_PaintEnd = 0;
	m_VscrollPos = 0;
}

// Дестркутор.
ChildWin::~ChildWin()
{
	Destroy();
}

// Установить конец строки вывода.
VOID ChildWin::Set_m_PaintEnd(long long End)
{
	m_PaintEnd = End;
}

// Установить начала строки вывода.
VOID ChildWin::Set_m_PaintBeg(long long Begin)
{
	m_PaintBeg = Begin;
}

// Получить дескриптор дочернего окна.
HWND ChildWin::Get_m_WndChild()
{
	return m_WndChild;
}

// Получить дескриптор окна Edit (директория).
HWND ChildWin::Get_m_Edit()
{
	return m_Edit;
}

// Получить дескриптор окна Button (кнопка).
HWND ChildWin::Get_m_Button()
{
	return m_Button;
}

// Передать дескриптор родительского окна.
void ChildWin::Set_hWnd(HWND parrent)
{
	m_hWnd = parrent;
}

// Задать текущее значение окон.
void ChildWin::Set_N(__in int Count_N)
{
	count_N = Count_N;
}

// Задать модуль окна.
void ChildWin::Set_hInst(__in HINSTANCE hInst)
{
	hInstance = hInst;
}