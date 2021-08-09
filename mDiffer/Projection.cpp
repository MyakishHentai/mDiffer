#include "Projection.h"

string Projection::GetState()
{
	return m_State;
};

VOID Projection::StartWork()
{
	m_StartWork = true;
};

BYTE Projection::GetProjFile(__in long long index)
{
    __try
	{
		return m_ProjFile[index];
	}
	__except (GetExceptionCode()) {
		puts("excepted code:");
		puts((char*)GetExceptionCode());
	}
}

wstring Projection::GetPath()
{
	return m_Path;
};

VOID Projection::SetPath(wstring x)
{
	this->m_Path = x;
};

long long Projection::GetFileSizing()
{
	return m_FileSizing;
};

DWORD Projection::GetBytesInBlock()
{
	return m_BytesInBlock;
};

VOID Projection::SetBytesInBlock(__in DWORD x)
{
	this->m_BytesInBlock = x;
};

BOOL Projection::CreateMapping()
{
	if (!m_StartWork)
	{
		return FALSE;
	}

	HANDLE hFile;

	do
	{
		hFile = CreateFileW(m_Path.c_str(), GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			m_State = "INVALID_HANDLE_VALUE";
			break;
		}

		m_FileMapping = CreateFileMapping(hFile,
			NULL, PAGE_READONLY,
			0, 0, NULL);

		if (m_FileMapping == NULL)
		{
			m_State = "FileMapping = NULL";
			break;
		}

		DWORD dwFileSizeHigh1;

		m_FileSizing = GetFileSize(hFile, &dwFileSizeHigh1);
		m_FileSizing += (((__int64)dwFileSizeHigh1) << 32);

		CloseHandle(hFile);
		m_State = CORRECT_PROJ;
		return TRUE;

	} while (0);

	CloseMap();
	CloseHandle(hFile);
	return FALSE;
}

BOOL Projection::CreateMapView(__in long long fileOffSet)
{
	m_ProjFile = (PBYTE)MapViewOfFile(m_FileMapping, FILE_MAP_READ,
		(DWORD)(fileOffSet >> 32),
		(DWORD)(fileOffSet & 0xffffffff),
		m_BytesInBlock);

	if (m_ProjFile != nullptr)
	{
		m_State = CORRECT_PROJ;
		return TRUE;
	}
	else
	{
		m_State = "m_ProjFile = nullptr";
		return FALSE;
	}

}

VOID Projection::UnmapView()
{
	UnmapViewOfFile(m_ProjFile);
	m_ProjFile = nullptr;
}

VOID Projection::CloseMap()
{
	if (m_FileMapping != INVALID_HANDLE_VALUE && m_FileSizing != INVALID_FILE_SIZE)
	{
		CloseHandle(m_FileMapping);
		m_FileMapping = INVALID_HANDLE_VALUE;
		m_FileSizing = 0;
	}
}

Projection::Projection()
{
	m_State = CORRECT_PROJ;
	m_StartWork = false;
	m_ProjFile = nullptr;
	m_FileSizing = 0;
	m_ProjFile = nullptr;
	m_Path = L"";
	m_FileMapping = nullptr;
	m_BytesInBlock = 0;
}

Projection::~Projection()
{
	UnmapView();
	CloseMap();
}


