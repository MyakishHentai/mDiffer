#include "Action.h"
#include "mDiffer.h"

Mask Action::m_Mask;
long long Action::m_ChildVScrollMax;
float Action::m_ChildCoeffVScroll;
int Action::cxClient;
int Action::cyClient;
HWND Action::m_hWnd;
HINSTANCE Action::hInstance;
static vector<ChildWin*> vectorChild;	// Вектор хранящий указатели на объекты класса ChildWin.
int num_Win = 0;						// Текущее количество окон для сравнения.
char text[] = { 0 };					// Буфер для немодального диалогового окна.
HWND m_Edit;							// Дескриптор поля ввода для диалогового окна.
HWND m_Dlg;								// Дескриптор диалогового окна.

// Основная функция регитрации окон и инициализаци их взимодействия.
int Action::Start(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
	MSG msg;
	// Зарезервировать память
	vectorChild.reserve(100);
	// Инициализация глобальных строк.
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(szCmdLine);
	char szAppName[] = "mDiffer";
	//Регистрация класса окна.
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW | DS_CENTER;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_MYICON));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wcex.lpszMenuName = szAppName;
	wcex.lpszClassName = szAppName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassEx(&wcex);

	WNDCLASS w;
	memset(&w, 0, sizeof(WNDCLASS));
	w.lpfnWndProc = ChildProc;
	w.hInstance = hInstance;
	w.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	w.lpszClassName = "Child";
	//w.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	w.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&w);

	const auto hMenu = CreateMenu();
	HMENU hMenuPopupPop = CreateMenu();

	// Всплывающее меню для подпунктов.  
	AppendMenu(hMenuPopupPop, MF_STRING, MENU_CHOICE_FILES_2, "2");
	AppendMenu(hMenuPopupPop, MF_STRING, MENU_CHOICE_FILES_3, "3");
	AppendMenu(hMenuPopupPop, MF_STRING, MENU_CHOICE_FILES_4, "4");
	AppendMenu(hMenuPopupPop, MF_STRING | MF_SEPARATOR, NULL, "");
	AppendMenu(hMenuPopupPop, MF_STRING, MENU_CHOICE_FILES_OTHER, "Другое значение...");

	// Всплывающее меню.
	HMENU hMenuPopup = CreateMenu();
	AppendMenu(hMenuPopup, MF_STRING, MENU_START, "Сравнить");
	AppendMenu(hMenuPopup, MF_POPUP, (UINT)hMenuPopupPop, "Выбрать значение 'N'");

	// Основное меню.
	AppendMenu(hMenu, MF_POPUP, (UINT)hMenuPopup, "Средства");
	AppendMenu(hMenu, MF_STRING, MENU_INFO, "Информация");
	AppendMenu(hMenu, MF_STRING, MENU_EXIT, "Exit");

	// Создание главного окна по центру экрана.
	m_hWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW | WS_HSCROLL,
		(GetSystemMetrics(SM_CXSCREEN) - MIN_WIDTH) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - MIN_HEIGHT) / 2,
		MIN_WIDTH, MIN_HEIGHT, nullptr, hMenu, hInstance, nullptr);


	if (!m_hWnd)
	{
		return FALSE;
	}

	// Передача исходных параметров для класса WinChild.
	ChildWin::Set_hWnd(m_hWnd);
	ChildWin::Set_hInst(hInstance);
	ChildWin::Set_N(num_Win);
	// Создание 2 рабочих окон при запуске.
	CorrectChoice(2);

	ShowWindow(m_hWnd, nCmdShow);
	UpdateWindow(m_hWnd);


	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

// Обработка команд.
int Action::WndCommand(__in WPARAM wParam)
{
	int wmId = LOWORD(wParam);
	if ((wmId >= EDIT_FIRST && (wmId <= 400))) {
		return 0; // OpenFile(wmId - EDIT_FIRST);
	}
	switch (wmId)
	{
		case MENU_CHOICE_FILES_2:
			return CorrectChoice(2);
		case MENU_CHOICE_FILES_3:
			return CorrectChoice(3);
		case MENU_CHOICE_FILES_4:
			return CorrectChoice(4);
		case MENU_CHOICE_FILES_OTHER:
			if (m_Dlg == NULL)
			{
				m_Dlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_MDIFFER_DIALOG), m_hWnd,(DLGPROC)DlgProc);
				ShowWindow(m_Dlg, SW_SHOW);
			}
			else
			{
				MessageBox(nullptr,
					"Окно уже открыто!",
					"Ошибка", MB_ICONASTERISK | MB_OK);
			}
			return 0;
		case MENU_START:
			Change(true);
			return 0;
		case MENU_INFO:
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), m_hWnd, About);
			return 0;
		case MENU_EXIT:
			DestroyWindow(m_hWnd);
			return 0;
		default:
		{
			return ChoiceFile(wParam);
		}
	}
}

// Смещение дочерних окон при измении позиции ползунка, масштабировании.
VOID Action::MoveWindows(__in int sbPos)
{
	// Отступ между окнами
	int step = 10;
	// Новая ширина окна
	int width = cxClient / 2 > MAX_WIDTH_CHILD ? MAX_WIDTH_CHILD - step : cxClient / 2 - step;
	for (auto i = 0; i < vectorChild.size(); i++)
	{
		MoveWindow(vectorChild[i]->Get_m_Button(), i * (width + step) + step - sbPos * (width + step), 10, BUTTON_WIDTH, BUTTON_HEIGHT, true);

		MoveWindow(vectorChild[i]->Get_m_Edit(), i * (width + step) + step + BUTTON_WIDTH + 10 - sbPos * (width + step), 10, width - BUTTON_WIDTH - 10 - step, EDIT_HEIGHT, true);

		MoveWindow(vectorChild[i]->Get_m_WndChild(), i * (width + step) + step - sbPos * (width + step), 60, width - step, cyClient - 70, true);
	}
}

// Обработчик сообщений главного окна.
LRESULT CALLBACK Action::WndProc(HWND m_hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int m_HscrollPos = 0;
	switch (message)
	{
	case WM_GETMINMAXINFO:
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = 400;
		lpMMI->ptMinTrackSize.y = 400;
		//lpMMI->ptMaxTrackSize.x = (MAX_WIDTH_CHILD) * vectorChild.size() + 10* (vectorChild.size()-1);
		//lpMMI->ptMaxTrackSize.y = GetSystemMetrics(SM_CYSCREEN)-30;
		return 0;
	}
	case WM_CREATE:
	{
		return 0;
	}
	case WM_SIZE:
	{
		cxClient = LOWORD(lParam);		// Ширина окна клиента
		cyClient = HIWORD(lParam);		// Высота окна клиента
		
		MoveWindows(m_HscrollPos);
		Change(false);
		ChildSetVScroll();
		return 0;
	}
	
	case WM_HSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			m_HscrollPos -= 1;
			break;
		case SB_LINEDOWN:
			m_HscrollPos += 1;
			break;
		case SB_PAGEUP:
			m_HscrollPos -= 2;
			break;
		case SB_PAGEDOWN:
			m_HscrollPos += 2;
			break;
		case SB_THUMBPOSITION:
			m_HscrollPos = HIWORD(wParam);
			break;
		default:
			break;
		}
		m_HscrollPos = max(0, min(m_HscrollPos, vectorChild.size() - 1));
		if (m_HscrollPos != GetScrollPos(m_hWnd, SB_HORZ))
		{
			SetScrollPos(m_hWnd, SB_HORZ, m_HscrollPos, TRUE);			
			//InvalidateRect(m_hWnd, NULL, TRUE);
			MoveWindows(m_HscrollPos);
			Change(false);
			//UpdateWindow(m_hWnd);
		}
		return 0;
	case WM_COMMAND:
	{
		SetFocus(m_hWnd);
		return WndCommand(wParam);
	}
	case WM_KEYDOWN:
	{
		return KeyboardEvents(wParam);
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(m_hWnd, message, wParam, lParam);
}

// Обработчик сообщений для окна "Другое значение".
BOOL CALLBACK Action::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		m_Edit = GetDlgItem(m_Dlg, IDC_EDIT1);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			GetDlgItemText(m_Dlg, IDC_EDIT1, text, 255);
			if (strlen(text) != 0)
			{
				int i = atoi(text);
				if (i < 2)
				{
					MessageBox(nullptr,
						"Количество окон должно быть >= 2!",
						"Ошибка", MB_ICONASTERISK | MB_OK);
				}
				else if (i >= 100)
				{
					MessageBox(nullptr,
						"Количество окон не может быть больше 100!",
						"Ошибка", MB_ICONASTERISK | MB_OK);
				}
				else
				{
					CorrectChoice(i);
					DestroyWindow(m_Dlg);
					m_Dlg = NULL;
					return TRUE;
				}		
			}
			else
			{
				MessageBox(nullptr,
					"Укажите значение!",
					"Ошибка", MB_ICONASTERISK | MB_OK);
			}
			break;
		}
		case IDCANCEL:
		{
			DestroyWindow(m_Dlg);
			m_Dlg = NULL;
			return TRUE;
		}
		}
		break;
	case WM_CLOSE:
	{
		DestroyWindow(m_Dlg);
		m_Dlg = NULL;
		return TRUE;
	}
	}
	return FALSE;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK Action::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Проверка на выбор количества окон.
int Action::CorrectChoice(int sel_num_Win)
{
	if (num_Win > sel_num_Win && vectorChild.size() != 0)
	{
		//Если окон больше, чем необходимо
		while (num_Win != sel_num_Win) {
			MoveWindows(vectorChild.size() - sel_num_Win);
			SetScrollRange(m_hWnd, SB_HORZ, 0, sel_num_Win - 1, FALSE);
			SetScrollPos(m_hWnd, SB_HORZ, sel_num_Win, TRUE);
			delete vectorChild[num_Win - 1];//->~ChildWin();
			vectorChild.pop_back();
			--num_Win;
			ChildWin::Set_N(vectorChild.size());
		}
		//Отключить скролл при наличии всего лишь 2 окон
		if (vectorChild.size() <= 2)
		{
			EnableScrollBar(m_hWnd, SB_HORZ, ESB_DISABLE_BOTH);
			MoveWindows(0);
		}
		Change(false);
	}
	else if (num_Win < sel_num_Win)
	{
		//Если окон меньше, чем необходимо
		int dif_num = sel_num_Win - num_Win;
		AddControls(m_hWnd, dif_num);
		num_Win = sel_num_Win;
	}
	else if (num_Win == sel_num_Win && vectorChild.size() == sel_num_Win)
	{
		//Если необходимое количество уже отрисовано
		MessageBox(nullptr,
			"Заданное количество уже выбрано",
			"Ошибка", MB_ICONEXCLAMATION | MB_OK);
	}
	UpdateWindow(m_hWnd);
	return 0;
}

// Создание объектов класса ChildWin и их расположение в окне.
void Action::AddControls(HWND m_hWnd, int Count_N)
{
	for (int i = num_Win; i < num_Win + Count_N; i++)
	{
		ChildWin* comboControl = new ChildWin(vectorChild.size());
		vectorChild.push_back(comboControl);
		*vectorChild[i]->GetProjection() = Projection();
	}
	MoveWindows(0);
	ChildSetVScroll();
	ChildWin::Set_N(vectorChild.size());
	if (vectorChild.size() <= 2) 
	{
		EnableScrollBar(m_hWnd, SB_HORZ, ESB_DISABLE_BOTH);
	}
	else
	{
		EnableScrollBar(m_hWnd, SB_HORZ, ESB_ENABLE_BOTH);
		SetScrollRange(m_hWnd, SB_HORZ, 0, static_cast<int>(vectorChild.size() - 1), FALSE);
		SetScrollPos(m_hWnd, SB_HORZ, 0, TRUE);
	}
	
}

// Возвращает директорию выбранного в диалоге файла.
wstring FileDialog(__in const HWND hwnd)
{
	static OPENFILENAMEW ofn;
	wchar_t szOpenFile[260];

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szOpenFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szOpenFile);
	ofn.lpstrFilter = L"All\0*.*\0Text\0*.*TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	GetOpenFileNameW(&ofn);
	return ofn.lpstrFile;
}

// Проверка состояния проецирования.
VOID Action::CheckState()
{
	string message;
	for (int i = 0; i < vectorChild.size(); i++)
	{
		if (vectorChild[i]->GetProjection()->GetState() != CORRECT_PROJ)
		{
			message = to_string(i + 1) + " Файл пустой";
			MessageBox(m_hWnd, message.c_str(), "Ошибка", MB_ICONINFORMATION);
		}
	}
};

// Запуск отрисовки и сравнения.
VOID Action::Change(__in bool fileWasChoice)
{
	static long long MemoryOfSize;

	if (fileWasChoice)
		//Был выбран другой файл
	{
		for (auto window : vectorChild)
		{
			window->Clear(TRUE);
		}
		m_Mask.SetLineOfFileMax(vectorChild);
		m_Mask.SetVScrollPos(1);
		ChildProc(vectorChild[0]->Get_m_WndChild(), WM_SET_VERTICAL_SCROLL, 0, 0);
		m_Mask.CreateMask(vectorChild);
		for (auto window : vectorChild)
		{
			if (window->GetProjection()->GetPath() == L"")
			{
				string message;
				int i = GetWindowLong(window->Get_m_WndChild(), GWL_ID) - 399;
				message = "ID:" + to_string(i) + " Файл не выбран";
				SetWindowTextA(window->Get_m_Edit(), message.c_str());
				window->Get_m_Edit();
				//MessageBox(m_hWnd, message.c_str(), "Не указан файл", MB_ICONINFORMATION);
				continue;
			}
			window->Print(m_Mask.GetMask(), m_Mask.GetVscrollPos());
		}
		MemoryOfSize = m_Mask.GetLineOfFileMax();
		CheckState();
		if (MemoryOfSize == NULL)
		{
			MessageBox(m_hWnd, "Выберите файл", "Ошибка", MB_ICONINFORMATION);
			return;
		}
		for (int i = 0; i < vectorChild.size(); i++)
		{
			EnableScrollBar(vectorChild[i]->Get_m_WndChild(), SB_VERT, ESB_ENABLE_BOTH);
		}
	}
	else
	{
		m_Mask.SetLineOfFileMax(vectorChild);

		if (MemoryOfSize != m_Mask.GetLineOfFileMax())
		{
			//Изменена длина текущего файла
			for (auto window : vectorChild)
			{
				window->Clear(TRUE);
			}
			m_Mask.SetVScrollPos(1);
			ChildProc(vectorChild[0]->Get_m_WndChild(), WM_SET_VERTICAL_SCROLL, 0, 0);
			MemoryOfSize = m_Mask.GetLineOfFileMax();
		}
		m_Mask.CreateMask(vectorChild);
		for (auto window : vectorChild)
		{
			if (window->GetProjection()->GetPath() == L"")
			{				
				continue;
			}
			window->Print(m_Mask.GetMask(), m_Mask.GetVscrollPos());
		}
	}
}

// Задать необходимые параметры и путь.
int Action::ChoiceFile(__in WPARAM wParam)
{
	wstring path = FileDialog(m_hWnd);
	if (path != L"")
	{
		vectorChild[LOWORD(wParam)]->GetProjection()->SetPath(path);
		SetWindowTextW(vectorChild[LOWORD(wParam)]->Get_m_Edit(), path.c_str());
		vectorChild[LOWORD(wParam)]->GetProjection()->StartWork();
		
		Change(true);
	}

	return 0;
}

// Обработка нажатий на клавиатуру.
int Action::KeyboardEvents(__in WPARAM wParam)
{
	long long m_VscrollPos = 0;		

	switch (wParam)
	{
	case VK_F1:
	{
		if (vectorChild.size() > 0)
		{
			ChoiceFile(0);
		}
		break;
	}
	case VK_F2:
	{
		if (vectorChild.size() > 1)
		{
			ChoiceFile(1);
		}
		break;
	}
	case VK_TAB:
	{
		Change(true);
		break;
	}
	case VK_ESCAPE:
	{
		PostQuitMessage(0);
		break;
	}
	case VK_UP:
	{
		m_VscrollPos = -1;
		break;
	}
	case VK_DOWN:
	{
		m_VscrollPos = 1;
		break;
	}
	//Page Up
	case VK_PRIOR:
	{
		m_VscrollPos = -(cyClient - 70) / CY_CHAR;
		break;
	}
	//Page Down
	case VK_NEXT:
	{
		m_VscrollPos = (cyClient - 70) / CY_CHAR;
		break;
	}
	/*default:
	{
		break;
	}*/
	}
	if (m_VscrollPos != 0)
	{
		VscrollProc(m_VscrollPos);
	}

	return 0;
};

// Устновка диапозона скрола.
int Action::ChildSetVScroll() {
	const auto lineMax = m_Mask.GetLineOfFileMax();
	m_ChildVScrollMax = max(0, lineMax + 1 - (cyClient-70) / CY_CHAR);

	int maxPos;
	if (lineMax > INT32_MAX)
	{
		maxPos = INT32_MAX;
	}
	else
	{
		maxPos = static_cast<int>(m_ChildVScrollMax);
	}

	for (auto i = 0; i < vectorChild.size(); i++)
	{
		SetScrollRange(vectorChild[i]->Get_m_WndChild(), SB_VERT, 0, maxPos, FALSE);
	}

	m_ChildCoeffVScroll = lineMax / INT32_MAX + (lineMax % INT32_MAX) * pow(INT32_MAX, -1);
	if (m_ChildCoeffVScroll < 1)
	{
		m_ChildCoeffVScroll = 1;
	}

	auto m_VscrollPos = min(m_Mask.GetVscrollPos(), m_ChildVScrollMax);
	m_Mask.SetVScrollPos(m_VscrollPos);

	if (m_VscrollPos / m_ChildCoeffVScroll < INT32_MAX)
	{
		m_VscrollPos /= m_ChildCoeffVScroll;
	}
	else
	{
		m_VscrollPos = INT32_MAX;
	}

	for (auto i = 0; i < vectorChild.size(); i++)
	{
		SetScrollPos(vectorChild[i]->Get_m_WndChild(), SB_VERT, m_VscrollPos, TRUE);
	}

	return 0;
};

// Обработка скрола.
VOID Action::VscrollProc(__inout long long vScrollInc)
{
	auto m_VscrollPos = m_Mask.GetVscrollPos();
	vScrollInc = max(
		-m_VscrollPos + 1,
		min(vScrollInc, m_ChildVScrollMax - m_VscrollPos)
	);

	if (vScrollInc != 0)
	{
		m_VscrollPos += vScrollInc;
		m_Mask.SetVScrollPos(m_VscrollPos);
		int scrollPos;

		if (m_VscrollPos / m_ChildCoeffVScroll < INT32_MAX)
		{
			scrollPos = m_VscrollPos / m_ChildCoeffVScroll;
		}
		else
		{
			scrollPos = INT32_MAX;
		}

		for (auto i = 0; i < vectorChild.size(); i++)
		{
			ScrollWindow(vectorChild[i]->Get_m_WndChild(), 0, -CY_CHAR * vScrollInc, NULL, NULL);
			SetScrollPos(vectorChild[i]->Get_m_WndChild(), SB_VERT, scrollPos, TRUE);
		}

		Change(false);
	}
};

// Инициализация скрола.
int Action::ChildVScroll(__in HWND hwnd, __in WPARAM wParam) {
	long long m_VscrollPos;					

	switch (LOWORD(wParam))
	{
	case SB_TOP:
		m_VscrollPos = m_Mask.GetVscrollPos();
		break;
	case SB_BOTTOM:
		m_VscrollPos = (m_ChildVScrollMax - m_Mask.GetVscrollPos());
		break;
	case SB_LINEUP:
		m_VscrollPos = -LINE_SCROLL;
		break;
	case SB_LINEDOWN:
		m_VscrollPos = LINE_SCROLL;
		break;
	case SB_PAGEUP:
		m_VscrollPos = -(m_ChildVScrollMax / PERCENT_SCROLL);
		break;
	case SB_PAGEDOWN:
		m_VscrollPos = (m_ChildVScrollMax / PERCENT_SCROLL);
		break;
	case SB_THUMBPOSITION:
	{
		SCROLLINFO si;
		ZeroMemory(&si, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_TRACKPOS;
		if (!GetScrollInfo(hwnd, SB_VERT, &si))
		{
			return 1;
		}
		const auto trackPos = si.nTrackPos;
		m_VscrollPos = (trackPos * m_ChildCoeffVScroll - m_Mask.GetVscrollPos());

		break;
	}
	case SB_THUMBTRACK:
	{
		SCROLLINFO si;
		ZeroMemory(&si, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_TRACKPOS;
		if (!GetScrollInfo(hwnd, SB_VERT, &si))
		{
			return 1;
		}
		const auto trackPos = si.nTrackPos;
		m_VscrollPos = (trackPos * m_ChildCoeffVScroll - m_Mask.GetVscrollPos());

		break;
	}
	default:
		m_VscrollPos = 0;
	}

	VscrollProc(m_VscrollPos);

	return 0;
};

// Обработка прокрутки колесика мышки.
int Action::ChildMouseWheel(__in WPARAM wParam) {
	long long m_VscrollPos;							//Изменение значения вертикальной скролл полосы		
	if ((short)HIWORD(wParam) < 0)
	{
		m_VscrollPos = MOUSE_SCROLL;
	}
	else
	{
		m_VscrollPos = -MOUSE_SCROLL;
	}

	VscrollProc(m_VscrollPos);

	return 0;
};

// Обработчик сообщений от дочерних окон.
LRESULT CALLBACK Action::ChildProc(__in HWND hwnd, __in UINT iMsg, __in WPARAM wParam, __in LPARAM lParam)
{
	switch (iMsg)
	{
	//Установка позиций строк для вывода
	case WM_SET_VERTICAL_SCROLL:
	{
		return ChildSetVScroll();
	}
	//Скролл по вертикали
	case WM_VSCROLL:
	{
		return ChildVScroll(hwnd, wParam);
	}
	//Скролл колесиком мыши
	case WM_MOUSEWHEEL:
	{
		return ChildMouseWheel(wParam);
	}
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}