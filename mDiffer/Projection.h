#ifndef H_Projection
#define H_Projection

#include "mDiffer.h"

// Класс проецирования из файла.
class Projection
{
public:
	Projection();
	~Projection();

	VOID SetPath(wstring x);						//Задаёт директорию файла.
	VOID SetBytesInBlock(__in DWORD x);				//Задает кол-во проецируемых байтов в 1 блоке.
	BOOL CreateMapping();							//Создание маппинга.
	BOOL CreateMapView(__in long long fileOffSet);	//Создание проецирования.
	VOID UnmapView();								//Освобождение проецирования.
	VOID CloseMap();								//Закрытие маппинга.
	wstring GetPath();								//Возвращает директорию файла.
	DWORD GetBytesInBlock();						//Возвращает кол-во проецируемых байтов в 1 блоке.
	long long GetFileSizing();						//Возвращает размер файла.
	BYTE GetProjFile(__in long long index);			//Возвращает проецирование из файла.
	string GetState();								//Возвращает состояние.
	VOID StartWork();								//Начало работы.

private:
	string m_State;									//Состояние объекта.
	wstring m_Path;									//Адрес файла.	
	HANDLE m_FileMapping;							//Маппинг файла.
	DWORD m_BytesInBlock;							//Количество проецируемых байтов в 1 файле.
	long long m_FileSizing;							//Размер файла.
	PBYTE m_ProjFile;								//Проецирование в файле.
	bool m_StartWork;								//Флаг начала работы.
};

#endif