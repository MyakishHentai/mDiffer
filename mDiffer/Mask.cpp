#include "Mask.h"
vector<bool> Mask::m_Mask;
SYSTEM_INFO Mask::m_SystemInfo;								
long long Mask::m_LineOfFileMax;							
long long Mask::m_VscrollPos;									
long long Mask::qwFileSizeMax;
long long Mask::m_PaintBeg;
long long Mask::m_PaintEnd;

// �������� ��������������� �����.
vector<bool>& Mask::GetMask() {
	return m_Mask;
}

// �������� �����������.
int GetMaxSystemPower(__in long long digit)
{
	return static_cast<int>(ceil(
		log(digit / BINARY + digit % BINARY * pow(BINARY, -1))
		/ log(DIGITS_SYSTEM)
	));
}

// �������� ������������ ���������� ����� � ������.
long long Mask::GetLineOfFileMax()
{
	return m_LineOfFileMax;
};

// �������� ������� �������.
long long Mask::GetVscrollPos()
{
	return m_VscrollPos;
};

// ���������� ������� �������.
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

// ���������� ���������� ����� ������ �� ������ ������������� ������� �����.
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
	//������� ����� ����������� �����
	m_LineOfFileMax = (qwFileSizeMax % SIZING_LINE == 0) ? qwFileSizeMax / SIZING_LINE : (qwFileSizeMax / SIZING_LINE) + 1;
}

// ��������� �����  � ������.
VOID Mask::CreateMask(__in vector<ChildWin*> Child)
{
	m_Mask.reserve(qwFileSizeMax);
	m_Mask.clear();
	m_PaintBeg = 0;
	m_PaintEnd = 0;
	GetSystemInfo(&m_SystemInfo);
	PAINTSTRUCT* ps = new PAINTSTRUCT[Child.size()];
	HDC* hdc = new HDC[Child.size()];
	// ������ ������� � ������.
	long long* fileSize = new long long[Child.size()];

	// ���������� bottom � ��������� ���������.
	long long bottom = 0;
	// ���������� top � ��������� ���������.
	long long top = 0;
	// ����� ������ ������.
	bool* stop = new bool[Child.size()];
	// ���� �������� �� ������������ ������
	//checkIdent = true;
	// �������� �������� � ������������� stop[]=false.

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

	//����������� ������������ �������
	const auto maxPow = GetMaxSystemPower(m_LineOfFileMax);
	for (auto window : Child)
	{
		window->Set_maxPow(maxPow);
	}

	//��������� ��������
	auto startOffset = m_PaintBeg * SIZING_LINE;

	//���������� ������� ����� �������������
	//���� ������ ������ �������������
	const long long k = startOffset / m_SystemInfo.dwAllocationGranularity;
	startOffset %= m_SystemInfo.dwAllocationGranularity;
	//�������� �������������
	auto fileOffSet = k * m_SystemInfo.dwAllocationGranularity;

	//������������ ���-�� ���� � �������������
	DWORD bytesInBlockMax = 0;

	//���������� ���������� ����� ����� � ������������� ������� ������ �����
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
			//������������� 
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

	//������� ����� � �������������� �����
	auto currentPrPos = startOffset;

	//������� ����� � �������� ������
	auto j = 0;

	//����� ����������� ������
	for (auto i = m_PaintBeg; i < m_PaintEnd; j++, currentPrPos++)
	{
		//������������� ���������
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

		//������� �� ����� ������
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
		//���������� ������			
		char* elem = new char[Child.size()];
		//�������� �� ����� �� ������� ��� ��������� �������� � ����� ������
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

		//������� �� ���������� �����������
		for (auto z = 0; z < Child.size(); z++)
		{
			//���� 1 ���� �������� => ����� �� �����
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

	//���������� ������
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