#pragma once
#include "CXML.h"

//3d max -> xml -> directx ....�Ľ̰� �ļ� Ŀ�ø��۾�...
//�밡�ٴ� �� ���ϰ� �����.
namespace XML
{
	class XMLParser
	{
	public:
		XMLParser();
		~XMLParser();

		bool XMLOpen(wchar_t* lpszFilename);


	private:
		CXML xml; //xml �ҷ�����
		int	_ParseInfo( );

	};
};