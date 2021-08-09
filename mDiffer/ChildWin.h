#include "framework.h"
#include "Projection.h"
#ifndef CHILDWIN_H
#define CHILDWIN_H

// Класс для создания комплекса дочерних окон - Кнопки(выбор файла), Поля Edit(директория), Дочернего окна(вывод файла).
class ChildWin
{
private:
	// Данные необходимые для создания окон.
	static HWND m_hWnd;
	static HINSTANCE hInstance;
	static int count_N;
	int maxPow;
	// Вспомогательные ункции создания и удаления окон.
	void CreateBox(int number_Obj);
	void Destroy();
	// Дескрипторы окон.											
	HWND m_Edit;
	HWND m_Button;
	HWND m_WndChild;
	Projection m_Projection;																			// Файл.	
	SYSTEM_INFO m_SystemInfo;																			// Системная информация.
	long long m_VscrollPos;																				// Позиция вертикальной полосы скролла.
	long long m_PaintBeg;																				// Номер строки начала рисования.
	long long m_PaintEnd;																				// Номер строки конца рисования.
	vector<char> Element;																				// Вектор элементов на вывод. 

public:
	VOID Set_maxPow(int value);																			// Установка разрядности.
	VOID Print(__in const vector<bool>& m_Mask, long long scroll);										// Сравнение содержимого и отображение.
	VOID Push_Element(char elem);																		// Добавление элемента на вывод в дочернее окно.
	VOID Clear(BOOL value);																				// Очистка окон.
	static VOID PrintNumber(__in long long pos, __in int maxPow, __in HDC hdc, __in int x, __in int y);	// Вывод номера элемента во все окна.
	static VOID PrintBinary(__in char sign, __in HDC hdc, __in int x, __in int y);						// Вывод элемента в бинарном виде.
	static VOID PrintText(__in char elem, __in HDC hdc, __in int x, __in int y);						// Вывод элемента.
	Projection* GetProjection();																		// Получение указателя на проецирование.	
	VOID Set_m_PaintEnd(long long End);																	// Получение номера строк
	VOID Set_m_PaintBeg(long long Begin);																// из класса Mask.
	ChildWin();																							// Конструкторы.
	ChildWin(int number_Obj);
	~ChildWin();																						// Деструктор.
	// Get\Set для дескрипторов и количества объектов.
	HWND Get_m_WndChild();
	HWND Get_m_Edit();
	HWND Get_m_Button();
	static void Set_hWnd(HWND hWnd0);
	static void Set_N(__in int Count_N);
	static void Set_hInst(__in HINSTANCE hInst);
};

#endif

