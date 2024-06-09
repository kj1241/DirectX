#ifndef DXUTIL_H
#define DXUTIL_H

// ��Ÿ ����� ���
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


#ifndef UNDER_CE
// �ý��� ������Ʈ���� ����� DirectX SDK ��θ� ��ȯ�մϴ�.
// SDK ��ġ ��.
HRESULT DXUtil_GetDXSDKMediaPathCch( TCHAR* strDest, int cchDest );
HRESULT DXUtil_GetDXSDKMediaPathCb( TCHAR* szDest, int cbDest );
HRESULT DXUtil_FindMediaFileCch( TCHAR* strDestPath, int cchDest, TCHAR* strFilename );
HRESULT DXUtil_FindMediaFileCb( TCHAR* szDestPath, int cbDest, TCHAR* strFilename );
#endif // !UNDER_CE

//���ڿ� ������Ʈ�� Ű�� �а� ���� ����� �Լ�
HRESULT DXUtil_WriteStringRegKey( HKEY hKey, TCHAR* strRegName, TCHAR* strValue );
HRESULT DXUtil_WriteIntRegKey( HKEY hKey, TCHAR* strRegName, DWORD dwValue );
HRESULT DXUtil_WriteGuidRegKey( HKEY hKey, TCHAR* strRegName, GUID guidValue );
HRESULT DXUtil_WriteBoolRegKey( HKEY hKey, TCHAR* strRegName, BOOL bValue );

HRESULT DXUtil_ReadStringRegKeyCch( HKEY hKey, TCHAR* strRegName, TCHAR* strDest, DWORD cchDest, TCHAR* strDefault );
HRESULT DXUtil_ReadStringRegKeyCb( HKEY hKey, TCHAR* strRegName, TCHAR* strDest, DWORD cbDest, TCHAR* strDefault );
HRESULT DXUtil_ReadIntRegKey( HKEY hKey, TCHAR* strRegName, DWORD* pdwValue, DWORD dwDefault );
HRESULT DXUtil_ReadGuidRegKey( HKEY hKey, TCHAR* strRegName, GUID* pGuidValue, GUID& guidDefault );
HRESULT DXUtil_ReadBoolRegKey( HKEY hKey, TCHAR* strRegName, BOOL* pbValue, BOOL bDefault );

// Ÿ�̸� �۾��� �����մϴ�. ���� ����� ����Ͻʽÿ�.
// TIMER_RESET - Ÿ�̸Ӹ� �缳���մϴ�.
// TIMER_START - Ÿ�̸Ӹ� �����մϴ�.
// TIMER_STOP - Ÿ�̸Ӹ� ����(�Ǵ� �Ͻ� ����)�մϴ�.
// TIMER_ADVANCE - Ÿ�̸Ӹ� 0.1�� �մ��ϴ�.
// TIMER_GETABSOLUTETIME - ���� �ý��� �ð��� �����ɴϴ�.
// TIMER_GETAPPTIME - ���� �ð��� �����ɴϴ�.
// TIMER_GETELAPSEDTIME - ����� �ð��� �����ɴϴ�. 
// TIMER_GETELAPSEDTIME ȣ��
enum TIMER_COMMAND { 
    TIMER_RESET, 
    TIMER_START, 
    TIMER_STOP, 
    TIMER_ADVANCE, 
    TIMER_GETABSOLUTETIME, 
    TIMER_GETAPPTIME, 
    TIMER_GETELAPSEDTIME 
};
FLOAT __stdcall DXUtil_Timer( TIMER_COMMAND command );

//CHAR, TCHAR, WCHAR ���ڿ� �� ��ȯ�� ���� UNICODE ����
HRESULT DXUtil_ConvertAnsiStringToWideCch( WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar );
HRESULT DXUtil_ConvertWideStringToAnsiCch( CHAR* strDestination, const WCHAR* wstrSource, int cchDestChar );
HRESULT DXUtil_ConvertGenericStringToAnsiCch( CHAR* strDestination, const TCHAR* tstrSource, int cchDestChar );
HRESULT DXUtil_ConvertGenericStringToWideCch( WCHAR* wstrDestination, const TCHAR* tstrSource, int cchDestChar );
HRESULT DXUtil_ConvertAnsiStringToGenericCch( TCHAR* tstrDestination, const CHAR* strSource, int cchDestChar );
HRESULT DXUtil_ConvertWideStringToGenericCch( TCHAR* tstrDestination, const WCHAR* wstrSource, int cchDestChar );
HRESULT DXUtil_ConvertAnsiStringToWideCb( WCHAR* wstrDestination, const CHAR* strSource, int cbDestChar );
HRESULT DXUtil_ConvertWideStringToAnsiCb( CHAR* strDestination, const WCHAR* wstrSource, int cbDestChar );
HRESULT DXUtil_ConvertGenericStringToAnsiCb( CHAR* strDestination, const TCHAR* tstrSource, int cbDestChar );
HRESULT DXUtil_ConvertGenericStringToWideCb( WCHAR* wstrDestination, const TCHAR* tstrSource, int cbDestChar );
HRESULT DXUtil_ConvertAnsiStringToGenericCb( TCHAR* tstrDestination, const CHAR* strSource, int cbDestChar );
HRESULT DXUtil_ConvertWideStringToGenericCb( TCHAR* tstrDestination, const WCHAR* wstrSource, int cbDestChar );

// �о�� ���
VOID DXUtil_LaunchReadme( HWND hWnd, TCHAR* strLoc = NULL );

// GUID�� ���ڿ��� ��ȯ
HRESULT DXUtil_ConvertGUIDToStringCch( const GUID* pGuidSrc, TCHAR* strDest, int cchDestChar );
HRESULT DXUtil_ConvertGUIDToStringCb( const GUID* pGuidSrc, TCHAR* strDest, int cbDestChar );
HRESULT DXUtil_ConvertStringToGUID( const TCHAR* strIn, GUID* pGuidOut );

// �߰� ����� �μ� ������ dxerr9.h�� �����ϼ���.
VOID    DXUtil_Trace( TCHAR* strMsg, ... );

#if defined(DEBUG) | defined(_DEBUG)
    #define DXTRACE           DXUtil_Trace
#else
    #define DXTRACE           sizeof
#endif

// CArrayList�� �����͸� �����ϴ� ����� ��Ÿ���ϴ�.
enum ArrayListType
{
    AL_VALUE,       // �׸� �����Ͱ� ��Ͽ� ����˴ϴ�.
    AL_REFERENCE,    // �׸� �����Ͱ� ��Ͽ� ����˴ϴ�.
};

//Ȯ�� ������ �迭
class CArrayList
{
protected:
    ArrayListType m_ArrayListType;
    void* m_pData;
    UINT m_BytesPerEntry;
    UINT m_NumEntries;
    UINT m_NumEntriesAllocated;

public:
    CArrayList( ArrayListType Type, UINT BytesPerEntry = 0 );
    ~CArrayList( void );
    HRESULT Add( void* pEntry );
    void Remove( UINT Entry );
    void* GetPtr( UINT Entry );
    UINT Count( void ) { return m_NumEntries; }
    bool Contains( void* pEntryData );
    void Clear( void ) { m_NumEntries = 0; }
};

//WinCE ���� ����

#ifdef UNDER_CE

#define CheckDlgButton(hdialog, id, state) ::SendMessage(::GetDlgItem(hdialog, id), BM_SETCHECK, state, 0)
#define IsDlgButtonChecked(hdialog, id) ::SendMessage(::GetDlgItem(hdialog, id), BM_GETCHECK, 0L, 0L)
#define GETTIMESTAMP GetTickCount
#define _TWINCE(x) _T(x)

__inline int GetScrollPos(HWND hWnd, int nBar)
{
	SCROLLINFO si;
	memset(&si, 0, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	if (!GetScrollInfo(hWnd, nBar, &si))
	{
		return 0;
	}
	else
	{
		return si.nPos;
	}
}

#else // !UNDER_CE

#define GETTIMESTAMP timeGetTime
#define _TWINCE(x) x

#endif // UNDER_CE

#endif // DXUTIL_H
