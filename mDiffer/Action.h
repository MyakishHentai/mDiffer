#include "framework.h"
#include "Mask.h"
#include "ChildWin.h"

#ifndef ACTION_H
#define ACTION_H

class Action
{
private:
	static Mask m_Mask;
	static long long m_ChildVScrollMax;												// Максимальная позиция вертикальной полосы скролла.
	static float m_ChildCoeffVScroll;												// Коэффициент отношения кол-ва скролл-позиций к максимальному значению в структуре = INT32_MAX.
	static int cxClient;															// Ширина клиентского окна.
	static int cyClient;															// Высота клиентского окна.

public:
	// Обработчик действий с основным окном.
	static LRESULT CALLBACK					WndProc(__in HWND m_hWnd, __in  UINT message, __in  WPARAM wParam, __in  LPARAM lParam);
	// Обработка команд.
	static int								WndCommand(__in WPARAM wParam);
	// Окно с краткой информацией.
	static INT_PTR CALLBACK					About(__in HWND hDlg, __in  UINT message, __in  WPARAM wParam, __in  LPARAM lParam);
	static BOOL CALLBACK					DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	// Проверка на количество созданных объектов для их корректного динамического отображения.
	static int								CorrectChoice(__in int sel_num_Win);
	// Создать объекты класса ChildWin.
	static void							    AddControls(__in HWND m_hWnd, __in int count_N);
	// "Точка входа" - параметры главного окна и классов окна, меню и т.д..
	static int								Start(__in HINSTANCE hInstance, __in  HINSTANCE hPrevInstance, __in  PSTR szCmdLine, __in  int nCmdShow);
	// Выбора файла и сохранение директории.
	static int								ChoiceFile(__in WPARAM wParam);		
	// Обработка действий с дочерним окном.
	static LRESULT CALLBACK					ChildProc(__in HWND hwnd, __in UINT iMsg, __in WPARAM wParam, __in LPARAM lParam);
	// Установка позиций строк для вывода.
	static int								ChildSetVScroll();
	// Скролл по вертикали в дочерних окнах.
	static int								ChildVScroll(__in HWND hwnd, __in WPARAM wParam);	
	// Скролл колесиком мыши.
	static int								ChildMouseWheel(__in WPARAM wParam);	
	// Обработка нажатий на клавиши.
	static int								KeyboardEvents(__in WPARAM wParam);	
	// Смещение окон в зависимости от позиции ползунка, масштабировании.
	static VOID								MoveWindows(__in int sbPos);											
	// Обработка изменений отображения.
	static VOID							    Change(__in bool fileWasChoice);	
	// Выполняет вертикальный скролл на параметр.
	static VOID								VscrollProc(__inout long long vScrollInc);		
	// Проверяет состояния файлов.
	static VOID								CheckState();														
	// Глобальные переменные:											
	
	static HINSTANCE hInstance;														// Дескриптор модуля.
	static HWND m_hWnd;																// Дескриптор окна.
};

#endif

