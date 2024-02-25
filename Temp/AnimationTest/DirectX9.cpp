#include "stdafx.h"
#include "DirectX9.h"

DirectX9::DirectX9()
{
    pCamera = new Camera;
}

DirectX9::~DirectX9()
{
    if (!pCamera)
        delete pCamera;
}

bool DirectX9::OnInit(HWND hWnd)
{
    if (!InitDevice(hWnd))
        return false;

    if (!InitGeometry())
        return false;
}

void DirectX9::OnUpdate()
{
}

void DirectX9::OnRender()
{
    if (NULL == pD3dDevice)
        return;

    /// �ĸ���ۿ� ������ �ʱ�ȭ 
    //pD3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0); //dx Ʃ�丮�󿡼��� �Ķ�������
    pD3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(200, 200, 200), 1.0f, 0);
    pD3dDevice->SetRenderState(D3DRS_FILLMODE, bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID); //�׸��� ���� ���� ���� ��� �Ҽ� �ִ� �κ��� �ƴѵ�?

    Animate(); //���ϸ��̼�

    /// ������ ����
    if (SUCCEEDED(pD3dDevice->BeginScene()))
    {
        /// ���� ������ ��ɵ��� ������ ��

        /// ������ ����
        pD3dDevice->EndScene();
    }

    /// �ĸ���۸� ���̴� ȭ������!
    pD3dDevice->Present(NULL, NULL, NULL, NULL);
}

void DirectX9::OnDestroy()
{
    if (pD3dDevice != NULL)
        pD3dDevice->Release();

    if (pD3D != NULL)
        pD3D->Release();
}

bool DirectX9::InitDevice(HWND hWnd)
{
    /// ����̽��� �����ϱ����� D3D��ü ����
    if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
        return false;

    // ����̽��� ������ ����ü
    // ������ ������Ʈ�� �׸����̱⶧����, �̹����� Z���۰� �ʿ��ϴ�.
    D3DPRESENT_PARAMETERS d3dpp;// ����̽� ������ ���� ����ü
    ZeroMemory(&d3dpp, sizeof(d3dpp)); // �ݵ�� ZeroMemory()�Լ��� �̸� ����ü�� ������ ������ �Ѵ�.
    d3dpp.Windowed = TRUE; // â���� ����
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;// ���� ȿ������ SWAPȿ��
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;// ���� ����ȭ�� ��忡 ���缭 �ĸ���۸� ����
    d3dpp.EnableAutoDepthStencil = TRUE; //���ٽ� ����
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16; // z ����



    /// ����̽��� ������ ���� �������� �����Ѵ�.
    /// 1. ����Ʈ ����ī�带 ���(��κ��� ����ī�尡 1�� �̴�.)
    /// 2. HAL����̽��� �����Ѵ�.(HW������ġ�� ����ϰڴٴ� �ǹ�)
    /// 3. ����ó���� ��� ī�忡�� �����ϴ� SWó���� �����Ѵ�.(HW�� �����Ұ�� ���� ���� ������ ����.)
    if (FAILED(pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pD3dDevice)))
    {
        return false;
    }

    /// ����̽� ���������� ó���Ұ�� ���⿡�� �Ѵ�.
    pD3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); // �⺻�ø�, CCW
    pD3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE); //Z���۱���� �Ҵ�.

    return true;
}

// ������Ʈ���� ���� �̰� directx12������ ���̴��� �ٲ������µ� ....
bool DirectX9::InitGeometry()
{
    InitMatrix();
    return true;
}

// ��� ����
void DirectX9::InitMatrix()
{
    // ���� ��� ����
    D3DXMatrixIdentity(&matWorld);
    pD3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    // �� ����� ���� (����ī�޶�)
    D3DXVECTOR3 vEyePt(0.0f, 150.0f, -(float)250.0f);
    D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
    D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
    pD3dDevice->SetTransform(D3DTS_VIEW, &matView);

    // ���� �������� ���
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f);
    pD3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 200.0f); // �������� �ø��� �������� ���

    pCamera->SetInitView(vEyePt, vLookatPt, vUpVec); // ī�޶� �ʱ�ȭ
}

void DirectX9::Animate()
{
    SetupLights();
}

void DirectX9::SetupLights()
{
    D3DXVECTOR3 vecDir; // ���⼺ ����(directional light)�� ���� ���� ����
    D3DLIGHT9 light; // ���� ����ü

    ZeroMemory(&light, sizeof(D3DLIGHT9));	// ����ü �ʱ�ȭ
    light.Type = D3DLIGHT_DIRECTIONAL; // ���� ����(��, ����, ����Ʈ����Ʈ
    
        // ������ ����� ���
    light.Diffuse.r = 1.0f;
    light.Diffuse.g = 1.0f;
    light.Diffuse.b = 1.0f;

    vecDir = D3DXVECTOR3(0, -1, 0);
    D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);// ���� ����

    light.Range = 1000.0f; // ������ �Ÿ�

    pD3dDevice->SetLight(0, &light); // ����̽��� 0�� �� ��ġ
    pD3dDevice->LightEnable(0, TRUE); // 0�� �� on
    pD3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE); // ������ on
    pD3dDevice->SetRenderState(D3DRS_AMBIENT, 0x00a0a0a0); // ȯ�汤��(ambient light)�� �� ����
}

