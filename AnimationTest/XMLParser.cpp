#include "stdafx.h"
#include "XMLParser.h"
#include "WinAPI.h"

XML::XMLParser::XMLParser()
{
}

XML::XMLParser::~XMLParser()
{
	xml.Close(); //�޸𸮿� �ȴ��� ������ ��������� �ѹ��� ����
}

bool XML::XMLParser::XMLOpen(wchar_t* lpszFilename)
{
	if (!xml.Open(lpszFilename))
	{
		WinAPI::pWinAPI->Log(L"can't open [%s] file.", lpszFilename);
		return false;
	}



}
