#include "stdafx.h"
#include "WinAPI.h"


_Use_decl_annotations_ //error c28213 �ذ� ��� ����: �����м� ���� ���� �ּ��� ���������� ���
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    WinAPI Windosws;
    //DirectX12EnginePipline Engine(1600, 900, L"DirectX12 Mini Engine");
    if (!Windosws.Init(/*&Engine,*/ hInstance, nCmdShow))  //�ʱ�ȭ �����ϸ� 
        return 0;   // 0���� ����
    return Windosws.Run(/*&Engine*/); //������ run �ڵ� ����
}
