#include "stdafx.h"
#include "WinAPI.h"

WinAPI* WinAPI::pWinAPI = nullptr;

LRESULT WinAPI::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return WinAPI::pWinAPI->WinProc(hWnd, msg, wParam, lParam);
}

WinAPI::WinAPI() : heigth(900), width(1600)//������
{
    pWinAPI = this;
    pDebugLog = new DebugLog(PlatformLog::WINDOW); //�α����� 
}

WinAPI::~WinAPI() //�Ҹ���
{
    pWinAPI = nullptr;

    if (pDebugLog != nullptr)
        delete pDebugLog;
}

bool WinAPI::Init(/*DirectX12Base* pDirectX,*/ HINSTANCE hInstance, int nCmdShow) //�ʱⰪ
{
    //����� �ŰԺ��� ���� 
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    //pDirectX->ParseCommandLineArgs(argv, argc);
    LocalFree(argv);
    // ������ Ŭ�� �ʱ�ȭ

    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WinAPI::WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"DirectX12MiniEngine";
    RegisterClassEx(&windowClass);

    RECT windowRect = { 0, 0, width, heigth }; //������ â����
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // â�� �ڵ� ����
    WinAPI_hwnd = CreateWindow(
        windowClass.lpszClassName,
        L"DirectX12 Mini Engine",//pDirectX->GetTitle(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,        // �θ�â ����
        nullptr,        // �޴� ������� ����
        hInstance,
        nullptr);
    //pDirectX); //���ν����� DirectXBase Ŭ���� �ּ� �ѱ��

   // if (!pDirectX->OnInit(WinAPI_hwnd)) //�ʱ�ȭ
   //     return false;

    pDirectX.OnInit();

    ShowWindow(WinAPI_hwnd, nCmdShow); //������ �����ֱ�
    return true;
}

//WinAPI����
int WinAPI::Run(/*DirectX12Base* pDirectX*/)
{
    // ���� ����
    MSG msg = { 0 };
    //pDirectX->GameTimeReset();

    while (msg.message != WM_QUIT) //�޽����� winAPI���ᰡ �ƴ϶��
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) // �޽����� ������ ó��
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else //�׷��� ������ ���ϸ��̼�/���� �۾��� ����
        {
            //pDirectX->GameTimeTick();

            //pDirectX->CalculateFrameStats();
           // pDirectX->OnUpdate(); //������Ʈ
           // pDirectX->OnRender(); //������

            pDirectX.OnUpdate();
            pDirectX.OnRender();
        }
    }

    pDirectX.OnDestroy();
   // pDirectX->OnDestroy(); //����
    UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);

    return static_cast<char>(msg.wParam);    // WM_QUIT �޽����� ��ȯ
}

//�ڵ鰪�� ��� ���ؼ�
HWND WinAPI::GetHwnd()
{
    return WinAPI_hwnd;
}

unsigned int WinAPI::GetWidth()
{
    return width;
}

unsigned int WinAPI::GetHeigth()
{
    return heigth;
}

//�� ���ν���
LRESULT WinAPI::WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //DirectX12Base* pDirectX = reinterpret_cast<DirectX12Base*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)); //�ڵ�� �Ѱ����� ����ϱ� ���ؼ�

    //�޽���
    switch (message)
    {
    case WM_CREATE: //â�� �����������
    {
        // ������ ����� DirectX12Base �� ����
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
    }
    return 0;

    case WM_KEYDOWN: //Ű��ư�� ��������
    /*    if (pDirectX)
        {
            pDirectX->OnKeyDown(static_cast<UINT8>(wParam));
        }*/
        return 0;

    case WM_KEYUP:  //Ű��ư��  �������
   /*     if (pDirectX)
        {
            pDirectX->OnKeyUp(static_cast<UINT8>(wParam));
        }*/
        return 0;

        //case WM_PAINT: //������� �ʴ� ����: ���Լ��� �ٽ� �׸��¿��ε� run()�Լ����� ó���ϰ� �ֱ� ����
        //    return 0;

    case WM_DESTROY: //�ı��Ǿ�����
        PostQuitMessage(0);
        return 0;
    }

    // ����Ʈ�� ��� ��� �޽��� ó��
    return DefWindowProc(hWnd, message, wParam, lParam);
}



//���� ���ڸ� ó�� <cstdarg>
bool WinAPI::Log(LPCWSTR fmt, ...)
{
    if (!pDebugLog)
        return false;

    va_list args;
    va_start(args, fmt);
    pDebugLog->Log(fmt, args);
    va_end(args);

    return true;
}
