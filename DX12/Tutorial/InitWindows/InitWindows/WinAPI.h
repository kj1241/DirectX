#pragma once
#include "DebugLog.h"


class WinAPI
{
public:
    WinAPI(); //������
    ~WinAPI(); //�Ҹ���
    bool Init(/*DirectX12Base* pDirectX,*/ HINSTANCE hInstance, int nCmdShow); //�ʱ�ȭ
    int Run(/*DirectX12Base* pDirectX*/); //����
    HWND GetHwnd(); //window �ڵ� ���
    unsigned int GetWidth();
    unsigned int GetHeigth();


    static WinAPI* pWinAPI;
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    bool Log(LPCWSTR fmt, ...);

protected:


private:
    LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); //������ ���ν���

    DebugLog* pDebugLog;
    WNDCLASSEX windowClass = { 0 };
    HWND WinAPI_hwnd; //������ �ڵ�

    unsigned int heigth;
    unsigned int width;


};
