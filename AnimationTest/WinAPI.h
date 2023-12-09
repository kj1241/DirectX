#pragma once
#include "DirectX9.h"
#include "DebugLog.h"


class DirectXPipline;

class WinAPI
{
public:
    WinAPI(); //������
    ~WinAPI(); //�Ҹ���
    bool Init(/*DirectX12Base* pDirectX,*/ HINSTANCE hInstance, int nCmdShow); //�ʱ�ȭ
    int Run(/*DirectX12Base* pDirectX*/); //����
    HWND GetHwnd(); //window �ڵ� ���
 
    static WinAPI* pWinAPI;

    LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); //������ ���ν���

    bool Log(LPCWSTR fmt, ...);

protected:


private:
    DebugLog* pDebugLog;
    DirectXPipline* pDirectX;
    WNDCLASSEX windowClass = { 0 };
    HWND WinAPI_hwnd; //������ �ڵ�

};
