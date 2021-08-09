#ifndef H_MASK
#define H_MASK

#include "ChildWin.h"

// Класс для создания общей маски по файлам.
class Mask
{
public:
	//static bool checkIdent;											// Проверка на идентичность файлов.
	static VOID SetLineOfFileMax(__in vector<ChildWin*> Child);			// Задает количество строк в выводе.
	static VOID SetVScrollPos(__in long long vscrollPos);				// Задает позицию вертикальной полосы скролла.
	static long long GetLineOfFileMax();								// Получить количество выводных строк в файле.	
	static long long GetVscrollPos();									// Получить позицию вертикальной полосы скролла.
	//bool checkIdent;													// Проверка на идентичность файлов.
	static VOID CreateMask(__in vector<ChildWin*> Child);				// Сравнение содержимого и отображение.
	static vector<bool>& GetMask();										// Возврат сгенерированной маски.

private:
	static SYSTEM_INFO m_SystemInfo;									// Системная информация.
	static::vector<bool> m_Mask;										// Вектор, хранящий вид элментов (false - не совпадают, true - совпадают).
	static long long m_PaintBeg;										// Номер строки начала рисования.
	static long long m_PaintEnd;										// Номер строки конца рисования.
	static long long m_LineOfFileMax;									// Количество выводных строк в файле.	
	static long long m_VscrollPos;										// Позиция вертикальной полосы скролла.
	static long long qwFileSizeMax;										// Максимальный размер файла.
};
#endif


