//{{NO_DEPENDENCIES}}
// Включаемый файл, созданный в Microsoft Visual C++.
// Используется mDiffer.rc
#ifndef H_MDIFFER
#define H_MDIFFER

#include <windows.h>
#include <string>
//#include "Default.h"
#include "resource.h"

using namespace std;

constexpr auto MIN_HEIGHT = 640;								//Минимальная высота главного окна.
constexpr auto MIN_WIDTH = 880;									//Минимальная ширина главного окна.
//ID для отслеживания событий:
constexpr auto MENU_CHOICE_FILES_2 = 202;
constexpr auto MENU_CHOICE_FILES_3 = 203;
constexpr auto MENU_CHOICE_FILES_4 = 204;
constexpr auto MENU_CHOICE_FILES_OTHER = 200;
constexpr auto MENU_START = 201;
constexpr auto MENU_INFO = 222;
constexpr auto MENU_EXIT = 999;
constexpr auto BUTTON_FIRST = 0;								//Соответсует [0-n] объектам вектора vectorChild. 
constexpr auto EDIT_FIRST = 300;
constexpr auto CHILD_FIRST = 400;
//Размеры дочерних окон:
constexpr auto BUTTON_WIDTH = 40;
constexpr auto BUTTON_HEIGHT = 40;
constexpr auto EDIT_WIDTH = 350;
constexpr auto EDIT_HEIGHT = 40;
constexpr auto CHILD_WIDTH = 400;
constexpr auto CHILD_HEIGHT = 470;

constexpr auto SIZING_LINE = 8;									//Длина линии вывода
constexpr auto PERCENT_SCROLL = 10;								//Процент скролла
constexpr auto LINE_SCROLL = 1;									//Величина смещения при скролле на 1 клик
constexpr auto MOUSE_SCROLL = 3;								//Величина смещения при колесом мыши
constexpr auto WM_SET_VERTICAL_SCROLL = 0x1111;					//Сообщение - установить позиции строк вывода
constexpr auto TRUE_TEXT_COLOR = RGB(0, 0, 0);					//Цвет при равенстве байтов в файлах
constexpr auto FALSE_TEXT_COLOR = RGB(255, 0, 0);				//Цвет при неравенстве байтов в файлах
constexpr auto NUMBERS_COLOR = RGB(0, 0, 0);					//Цвет цифр порядка
constexpr auto BINARY = 2;										//Бинарный вывод
constexpr auto DIGITS_SYSTEM = SIZING_LINE * BINARY;			//Система отсчета позиций
constexpr auto LENGTH_BINARY = 3;								//Количество символов в бинарном выводе
constexpr auto BINARY_BARRIER = 2;								//Барьер между бинарным и обычным выводом
constexpr auto CX_CHAR = 7;										//Ширина символа
constexpr auto CX_CAPS = 10;									//Caps-Ширина
constexpr auto CY_CHAR = 16;									//Высота символа
constexpr auto SPACE_STANDARD = 10;								//Длина пробела
constexpr auto CORRECT_PROJ = "Сorrect";						//Корректное состояние файла

//Параметры, вычисляемые на основании максимальной длины вывода, для ограничения растяжения:
constexpr auto MAX_WIDTH_CHILD = SIZING_LINE * 50 + 150;
constexpr auto MAX_WIDTH_EDIT = MAX_WIDTH_CHILD - 50;


#define IDS_APP_TITLE			1000

//#define IDC_EDIT1               1111
#define IDR_MAINFRAME			1001
#define IDD_MDIFFER_DIALOG		1002
#define IDD_ABOUTBOX			1003
#define IDM_ABOUT				1004
#define IDM_EXIT				1105
#define IDI_MDIFFER				1006
#define IDI_SMALL				1007
#define IDC_MDIFFER				1008
#define IDC_MYICON				1009
#ifndef IDC_STATIC
#define IDC_STATIC				-1
#endif
// Следующие стандартные значения для новых объектов
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS

#define _APS_NO_MFC					130
#define _APS_NEXT_RESOURCE_VALUE	129
#define _APS_NEXT_COMMAND_VALUE		32771
#define _APS_NEXT_CONTROL_VALUE		1000
#define _APS_NEXT_SYMED_VALUE		110
#endif
#endif

#endif



