#pragma once
#include "DirectXPipline.h"
#include "Camera.h"

class DirectX9:public DirectXPipline
{

public:

    DirectX9();
    virtual ~DirectX9();

    bool OnInit(HWND hWnd) override;
    void OnUpdate() override;
    void OnRender() override;
    void OnDestroy() override;

private:
    bool InitDevice(HWND hWnd);
    bool InitGeometry();
    void InitMatrix();
    void Animate();
    void SetupLights();

    Camera* pCamera;

    LPDIRECT3D9 pD3D = nullptr; /// D3D ����̽��� ������ D3D��ü����
    LPDIRECT3DDEVICE9 pD3dDevice = nullptr; /// �������� ���� D3D����̽�

    D3DXMATRIXA16 matWorld; //��Ʈ���� ������ǥ
    D3DXMATRIXA16 matView; //��Ʈ���� ����ǥ
    D3DXMATRIXA16 matProj; //��Ʈ���� ������Ʈ��ǥ

    bool bWireframe = false; //�׸�������

};