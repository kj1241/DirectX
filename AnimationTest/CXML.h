#pragma once
#include <msxml2.h>
#include <comdef.h> // 

//XML Ŭ����
//�ᱹ xml�� 


namespace XML
{
	//������ �׳� �ؽ�Ʈ�ε� �˻��� �������ִϱ� ����...
	class CXML
	{
	public:
		CXML();
		~CXML();
		bool Open(LPCWSTR url);
		void Close();
		IXMLDOMNodeList* FindElement(LPCTSTR strElement); //���� ã��
		IXMLDOMNodeList* FindElement(IXMLDOMNode* pNode, LPCTSTR strElement); //��� ã��
		bool GetElementText(IXMLDOMNode* pNode, LPSTR strRet); //������ �ý�Ʈ �ֱ�
		int	GetAttributeText(IXMLDOMNode* pNode, LPSTR strAttrName, LPSTR strRet);

	private:
		IXMLDOMDocument* pXMLDoc; 
		wchar_t sizeFilename[MAX_PATH];
	};;
};