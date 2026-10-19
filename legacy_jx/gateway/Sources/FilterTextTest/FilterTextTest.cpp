// FilterTextTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <iomanip>

#include <ios>
#include <iosfwd>
//---------------------
#include "..\FilterText\FilterText.h"
#include <Shellapi.h>

using namespace std;

typedef HRESULT (*LPFNCREATETEXTFILTER)(ITextFilter** ppTextFilter);
ITextFilter* pTextFilter;

void AddFilterWords(char *szFilterWordFile)
{
	if (NULL == pTextFilter)
		return;

	std::ifstream ifs(szFilterWordFile,ios::_Nocreate);
	if(!ifs)
	{
		std::cout<<"Can't Open FilterWordFile: "<<szFilterWordFile<<"!"<<std::endl;
		return;
	}
	char szLine[1024];
	while(!ifs.eof())
	{
		ifs.getline(szLine, 1023);
		if(ifs.bad())
		{
			std::cout<<"Read file error!"<<szFilterWordFile<<"!"<<std::endl;
			return;
		}
		pTextFilter->AddExpression(szLine);
	}
}

void DoTest(char *szTestFile)
{
	if (NULL == pTextFilter)
		return;

	std::ifstream ifs(szTestFile, ios::_Nocreate);
	if(!ifs)
	{
		std::cout<<"Can't Open FilterTestFile: "<<szTestFile<<"!"<<std::endl;
		return;
	}

	std::ofstream ofs("result.txt");
	if(!ofs)
	{
		std::cout<<"Can't create ResultFile: result.txt!"<<std::endl;
		return;
	}

	char szLine[1024];
	while(!ifs.eof())
	{
		ifs.getline(szLine, 1023);
		if(ifs.bad())
		{
			std::cout<<"Read file error!"<<szTestFile<<"!"<<std::endl;
			return;
		}

		if(pTextFilter->IsTextPass(szLine))
		{
			ofs<<"Pass "<<szLine<<std::endl;
		}
		else
		{
			ofs<<"Filt "<<szLine<<std::endl;
		}
	}

	ShellExecute(NULL, "open", "result.txt", NULL, NULL, SW_SHOW);
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		std::cout<<"Usage: FilterTextTest 过滤信息文件 要测试的文本文件!"<<std::endl;
		return FALSE;
	}

	HMODULE m_hFilterText;
	LPFNCREATETEXTFILTER lpfnCreateTextFilter;

	if (!(m_hFilterText = ::LoadLibrary("FilterText.dll")))
	{
		std::cout<<"Can't load FilterText.dll!"<<std::endl;
		return FALSE;
	}

	if (!(lpfnCreateTextFilter = (LPFNCREATETEXTFILTER)GetProcAddress(m_hFilterText, "CreateTextFilter")))
	{
		FreeLibrary(m_hFilterText);
		std::cout<<"Can't find function CreateTextFilter!"<<std::endl;
		return FALSE;
	}
	
	if (FAILED(lpfnCreateTextFilter(&pTextFilter)))
	{
		FreeLibrary(m_hFilterText);
		std::cout<<"CreateTextFilter failed!"<<std::endl;
		return FALSE;
	}

	AddFilterWords(argv[1]);
	DoTest(argv[2]);

	FreeLibrary(m_hFilterText);

	return 0;
}



