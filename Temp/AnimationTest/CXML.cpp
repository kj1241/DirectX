#include "stdafx.h"
#include "CXML.h"

XML::CXML::CXML():pXMLDoc(nullptr)
{
	//CoInitialize(nullptr);
	//CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_ALL, IID_IXMLDOMDocument, (void**)&pXMLDoc);
}

XML::CXML::~CXML()
{
	CoUninitialize();
}

bool XML::CXML::Open(LPCWSTR url)
{
	short sResult=0; // ���
	try
	{
		//������ ��ȿ���� Ȯ���ϴ¿� xml�� ã���� ������
		std::ifstream  file(url, std::ios::in); //�б�����
		if (!file.is_open())
			return false;
		file.close();

		pXMLDoc->put_async(false); //�񵿱������� �ε�x
		_bstr_t varString = (LPCTSTR)url; //VARIANT ���·� ��ȯ ... c++17�� �ش��������ʳ� �ϴ� �н�
		VARIANT path;
		path.vt = VT_BSTR;
		path.bstrVal = varString;

		int hr = pXMLDoc->load(path, &sResult);
		if (hr < 0)
			return false;
	}
	catch (...)
	{
		pXMLDoc->Release(); //
		pXMLDoc = nullptr;
		return false;
	}
	return true;
}

void XML::CXML::Close()
{
	pXMLDoc->Release();
}

IXMLDOMNodeList* XML::CXML::FindElement(LPCTSTR strElement)
{
	IXMLDOMNodeList* pNodeList = nullptr; //��� ����
	if (pXMLDoc == nullptr)
		return nullptr;

	try
	{
		_bstr_t bstrPath = strElement; //��������ָ� wchar_t���� VARIANT ��ȯ���ؼ� ��ã��
		pXMLDoc->selectNodes(bstrPath, &pNodeList);
	}
	catch (...)
	{

	}
	return pNodeList;
}

IXMLDOMNodeList* XML::CXML::FindElement(IXMLDOMNode* pNode, LPCTSTR strElement)
{
	IXMLDOMNodeList* pNodeList = nullptr; //��� ����
	if (pXMLDoc == nullptr)
		return nullptr;

	try
	{
		_bstr_t bstrPath = strElement;
		pNode->selectNodes(bstrPath, &pNodeList);
	}
	catch (...)
	{
	}

	return pNodeList;
}

bool XML::CXML::GetElementText(IXMLDOMNode* pNode, LPSTR strRet)
{
	BSTR bstr = nullptr;
	pNode->get_text(&bstr); 
	//const char_t* strResult = _bstr_t(bstr, false);
	const wchar_t* strResult = _bstr_t(bstr, false); //BSTR�� const wchar*�� ��ȯ�ϴ� �κ�

	
	size_t convertedChars = 0; 
	//strncpy(strRet, strResult, 128);
	wcstombs_s(&convertedChars, strRet, 127, strResult, _TRUNCATE); //Unicode ��ȯ�� �ٷ�� ���� wcstombs ���
	::SysFreeString(bstr); // BSTR ����

	return true;
}

int XML::CXML::GetAttributeText(IXMLDOMNode* pNode, LPSTR strAttrName, LPSTR strRet)
{
	wchar_t wstrAttr[128];
	IXMLDOMNode* pAttrNode = nullptr;
	IXMLDOMNamedNodeMap* pMap = nullptr;
	VARIANT	varValue; //c++17���°͵� �� ��������?

	try
	{
		int n = mbstowcs(wstrAttr, strAttrName, 128); //��Ƽ����Ʈ ���ڿ��� ���ϵ����Ʈ ���ڿ��� ��ȯ
		pNode->get_attributes(&pMap); //���� �Ӽ� ������
		pMap->getNamedItem(wstrAttr, &pAttrNode); //�Ӽ� ��� ������
		pAttrNode->get_nodeValue(&varValue); // �޼��� �Ӽ� �� ������

			
		//const char* strResult = _bstr_t(varValue.bstrVal, false);
		//strncpy(strRet, strResult, 127); // 127�� ���� �����÷ο� �����Ϸ��� �̵����� ��ߵ�

		wcstombs(strRet, varValue.bstrVal, 128); //��Ƽ����Ʈ�� ���ڿ��� ��ȯ�ϰ� ����

		//��ũ�� �����ߵǳ�?
		if (pAttrNode != nullptr)
		{
			pAttrNode->Release();
			pAttrNode = nullptr;
		}

		if (pMap != nullptr)
		{
			pMap->Release();
			pMap = nullptr;
		}

	}
	catch (...)
	{
		if (pAttrNode != nullptr)
		{
			pAttrNode->Release();
			pAttrNode = nullptr;
		}

		if (pMap != nullptr)
		{
			pMap->Release();
			pMap = nullptr;
		}
		return false;
	}

	return true;
}


