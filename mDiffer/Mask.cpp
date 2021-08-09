#include "Mask.h"
vector<bool> Mask::m_Mask;
SYSTEM_INFO Mask::m_SystemInfo;								
long long Mask::m_LineOfFileMax;							
long long Mask::m_VscrollPos;									
long long Mask::qwFileSizeMax;
long long Mask::m_PaintBeg;
long long Mask::m_PaintEnd;

// Получить сгенерированную маску.
vector<bool>& Mask::GetMask() {
	return m_Mask;
}

// Получить разрядность.
int GetMaxSystemPower(__in long long digit)
{
	return static_cast<int>(ceil(
		log(digit / BINARY + digit % BINARY * pow(BINARY, -1))
		/ log(DIGITS_SYSTEM)
	));
}

// Получить максимальное количество строк в выводе.
long long Mask::GetLineOfFileMax()
{
	return m_LineOfFileMax;
};

// Получить позицию скролла.
long long Mask::GetVscrollPos()
{
	return m_VscrollPos;
};

// Установить позицию скролла.
VOID Mask::SetVScrollPos(__in long long vscrollPos)
{
	if (vscrollPos > 0)
	{
		m_VscrollPos = vscrollPos;
	}
	else
	{
		m_VscrollPos = 1;
	}
};

// Установить количество линий вывода на основе максимального размера файла.
VOID Mask::SetLineOfFileMax(__in vector<ChildWin*> Child)
{
	for (auto i = 0; i < Child.size(); i++)
	{
		if (Child[i]->GetProjection()->CreateMapping())
		{
			const auto qw_file_size = Child[i]->GetProjection()->GetFileSizing();
			if (qwFileSizeMax < qw_file_size)
			{
				qwFileSizeMax = qw_file_size;
			}
		}
		Child[i]->GetProjection()->CloseMap();
	}
	//находим длину наибольшего файла
	m_LineOfFileMax = (qwFileSizeMax % SIZING_LINE == 0) ? qwFileSizeMax / SIZING_LINE : (qwFileSizeMax / SIZING_LINE) + 1;
}

// Генерация маски  и мапинг.
VOID Mask::CreateMask(__in vector<ChildWin*> Child)
{
	m_Mask.reserve(qwFileSizeMax);
	m_Mask.clear();
	m_PaintBeg = 0;
	m_PaintEnd = 0;
	GetSystemInfo(&m_SystemInfo);
	PAINTSTRUCT* ps = new PAINTSTRUCT[Child.size()];
	HDC* hdc = new HDC[Child.size()];
	// Размер остатка в файлах.
	long long* fileSize = new long long[Child.size()];

	// Наибольший bottom в структуре рисования.
	long long bottom = 0;
	// Наибольший top в структуре рисования.
	long long top = 0;
	// Флаги концов файлов.
	bool* stop = new bool[Child.size()];
	// Флаг проверки на идентичность файлов
	//checkIdent = true;
	// Создание маппинга и инициализация stop[]=false.

	for (auto i = 0; i < Child.size(); i++)
	{
		hdc[i] = BeginPaint(Child[i]->Get_m_WndChild(), &ps[i]);
		if (Child[i]->GetProjection()->CreateMapping())
		{
			fileSize[i] = Child[i]->GetProjection()->GetFileSizing();
			stop[i] = false;
		}
		else
		{
			fileSize[i] = 0;
			stop[i] = true;
		}

		if (bottom < ps[i].rcPaint.bottom)
		{
			bottom = ps[i].rcPaint.bottom;
		}
		if (top < ps[i].rcPaint.top)
		{
			top = ps[i].rcPaint.top;
		}
	}

	m_PaintBeg = max(0, m_VscrollPos + top / CY_CHAR - 1);
	m_PaintEnd = min(m_LineOfFileMax, m_VscrollPos + bottom / CY_CHAR);

	for (auto window : Child)
	{
		window->Set_m_PaintBeg(m_PaintBeg);
		window->Set_m_PaintEnd(m_PaintEnd);
	}

	//Разрядность максимальной позиции
	const auto maxPow = GetMaxSystemPower(m_LineOfFileMax);
	for (auto window : Child)
	{
		window->Set_maxPow(maxPow);
	}

	//Начальное смещение
	auto startOffset = m_PaintBeg * SIZING_LINE;

	//Нахождение нужного блока проецирования
	//Блок памяти начала проецирования
	const long long k = startOffset / m_SystemInfo.dwAllocationGranularity;
	startOffset %= m_SystemInfo.dwAllocationGranularity;
	//Смещение проецирования
	auto fileOffSet = k * m_SystemInfo.dwAllocationGranularity;

	//Максимальное кол-во байт в проецировании
	DWORD bytesInBlockMax = 0;

	//Уменьшение оставшейся длины файла и проецирование остатка особой длины
	for (auto i = 0; i < Child.size(); i++)
	{
		fileSize[i] -= fileOffSet;
		if (fileSize[i] <= 0)
		{
			fileSize[i] = 0;
			stop[i] = true;
		}

		if (!stop[i])
		{
			if (fileSize[i] < m_SystemInfo.dwAllocationGranularity)
			{
				Child[i]->GetProjection()->SetBytesInBlock((DWORD)fileSize[i]);
			}
			else
			{
				Child[i]->GetProjection()->SetBytesInBlock(m_SystemInfo.dwAllocationGranularity);
			}
			//Проецирование 
			if (Child[i]->GetProjection()->CreateMapView(fileOffSet))
			{
				if (bytesInBlockMax < Child[i]->GetProjection()->GetBytesInBlock())
				{
					bytesInBlockMax = Child[i]->GetProjection()->GetBytesInBlock();
				}
			}
			else
			{
				stop[i] = true;
			}
		}
		else
		{
			Child[i]->GetProjection()->SetBytesInBlock(0);
		}
	}

	//Позиция байта в проецированном блоке
	auto currentPrPos = startOffset;

	//Позиция байта в выходной строке
	auto j = 0;

	//Вывод содержимого файлов
	for (auto i = m_PaintBeg; i < m_PaintEnd; j++, currentPrPos++)
	{
		//проецирование кончилось
		if (currentPrPos == bytesInBlockMax)
		{
			for (auto z = 0; z < Child.size(); z++)
			{
				Child[z]->GetProjection()->UnmapView();
				fileSize[z] -= Child[z]->GetProjection()->GetBytesInBlock();
				if (fileSize[z] <= 0)
				{
					fileSize[z] = 0;
					stop[z] = true;
				}
				if (!stop[z])
				{
					if (fileSize[z] < m_SystemInfo.dwAllocationGranularity)
					{
						Child[z]->GetProjection()->SetBytesInBlock((DWORD)fileSize[z]);
					}
					else
					{
						Child[z]->GetProjection()->SetBytesInBlock(m_SystemInfo.dwAllocationGranularity);
					}
					if (!Child[z]->GetProjection()->CreateMapView(fileOffSet))
					{
						stop[z] = true;
					}
				}
				else
				{
					Child[z]->GetProjection()->SetBytesInBlock(0);
				}
			}
			fileOffSet += bytesInBlockMax;

			bytesInBlockMax = 0;
			for (auto z = 0; z < Child.size(); z++)
			{
				if (bytesInBlockMax < Child[z]->GetProjection()->GetBytesInBlock())
				{
					bytesInBlockMax = Child[z]->GetProjection()->GetBytesInBlock();
				}
			}

			currentPrPos = 0;
		}

		//Переход на новую строку
		if (j == SIZING_LINE)
		{
			j = 0;
			i++;
		}
		bool flag = true;
		for (auto z = 0; z < Child.size(); z++)
		{	
			if (!stop[z])
			{
				flag = false;
			}
		}
		if (flag)
		{
			break;
		}
		//Содержимое файлов			
		char* elem = new char[Child.size()];
		//Проверка на выход за границу при сравнении большого с малым файлом
		for (auto z = 0; z < Child.size(); z++)
		{
			if (currentPrPos >= fileSize[z])
			{
				stop[z] = true;
			}
			else
			{
				if (!stop[z])
				{
					elem[z] = (char)Child[z]->GetProjection()->GetProjFile(currentPrPos);
					Child[z]->Push_Element(elem[z]);
				}
			}
		}

		//Усливия не совпадения содержимого
		for (auto z = 0; z < Child.size(); z++)
		{
			//Если 1 файл кончился => файлы не равны
			if (stop[z])
			{
				m_Mask.push_back(false);
				break;
			}
			if (z + 1 == Child.size())
			{
				for (auto c = 0; c < Child.size() - 1; c++)
				{
					if (elem[c] != elem[c + 1])
					{
						m_Mask.push_back(false);
						break;
					}
					else
					{
						m_Mask.push_back(true);
						break;
					}
				}
			}
		}
		delete[] elem;
	}

	//Завершение работы
	for (auto i = 0; i < Child.size(); i++)
	{
		Child[i]->GetProjection()->UnmapView();
		Child[i]->GetProjection()->CloseMap();
		EndPaint(Child[i]->Get_m_WndChild(), &ps[i]);
		Child[i]->Clear(FALSE);
	}
	delete[] fileSize;
	delete[] stop;
}