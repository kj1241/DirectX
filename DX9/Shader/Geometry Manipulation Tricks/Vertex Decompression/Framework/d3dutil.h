#ifndef D3DUTIL_H
#define D3DUTIL_H
#include <D3D9.h>
#include <D3DX9Math.h>

// D3DMATERIAL9 ������ �ʱ�ȭ�Ͽ� Ȯ�� �� �ֺ� ȯ���� �����մϴ�.
// ����. ���� �Ǵ� �ݻ� ������ �������� �ʽ��ϴ�.
VOID D3DUtil_InitMaterial( D3DMATERIAL9& mtrl, FLOAT r=0.0f, FLOAT g=0.0f, FLOAT b=0.0f, FLOAT a=1.0f );

//D3DLIGHT ������ �ʱ�ȭ�Ͽ� ���� ��ġ�� �����մϴ�. �׸�ŭ Ȯ�� ������ ���, �ݻ籤 �� �ֺ� ������ ���������� �����˴ϴ�.
VOID D3DUtil_InitLight( D3DLIGHT9& light, D3DLIGHTTYPE ltType, FLOAT x=0.0f, FLOAT y=0.0f, FLOAT z=0.0f );

//// ����: �ؽ�ó�� �����ϴ� ����� �Լ��Դϴ�. ���� ��Ʈ ��θ� Ȯ���ϰ�, �׷� ���� DXSDK �̵�� ���(�ý��� ������Ʈ���� ������ ���)�� �õ��մϴ�.
HRESULT D3DUtil_CreateTexture( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* strTexture, LPDIRECT3DTEXTURE9* ppTexture, D3DFORMAT d3dFormat = D3DFMT_UNKNOWN );

//ť����� �鿡 �������ϱ� ���� �� ��Ʈ������ ��ȯ�մϴ�.
D3DXMATRIX D3DUtil_GetCubeMapViewMatrix( DWORD dwFace );

//â Ŀ���� ���� �Ͻõ� ȸ���� ���� ���ʹϾ��� ��ȯ�մϴ�.
//��ġ.
D3DXQUATERNION D3DUtil_GetRotationFromCursor( HWND hWnd,
                                              FLOAT fTrackBallRadius=1.0f );
//hCursor�� ������� D3D ��ġ�� ���� Ŀ���� �����ϰ� �����մϴ�.
HRESULT D3DUtil_SetDeviceCursor( LPDIRECT3DDEVICE9 pd3dDevice, HCURSOR hCursor, BOOL bAddWatermark );

//�־��� D3DFORMAT�� ���� ���ڿ��� ��ȯ�մϴ�.
//bWithPrefix�� ���ڿ��� "D3DFMT_"�� ���ԵǾ�� �ϴ��� ���θ� �����մϴ�.
TCHAR* D3DUtil_D3DFormatToString( D3DFORMAT format, bool bWithPrefix = true );

class CD3DArcBall
{
    INT            m_iWidth;   // ArcBall�� â �ʺ�
    INT            m_iHeight;  // ArcBall â ����
    FLOAT          m_fRadius;  // ȭ�� ��ǥ�� ǥ�õ� ArcBall�� �ݰ�
    FLOAT          m_fRadiusTranslation;    // Ÿ���� �̵��ϱ� ���� ArcBall�� �ݰ�

    D3DXQUATERNION m_qDown;    // ��ư�� ������ ���� ���ʹϾ�
    D3DXQUATERNION m_qNow;               // ���� �巡�׿� ���� ���� ���ʹϾ�
    D3DXMATRIX  m_matRotation;        // ��ȣ�� ���⿡ ���� ���
    D3DXMATRIX  m_matRotationDelta;    // ��ȣ�� ���⿡ ���� ���
    D3DXMATRIX  m_matTranslation;    // ��ȣ���� ��ġ�� ���� ���
    D3DXMATRIX  m_matTranslationDelta;    // ��ȣ���� ��ġ�� ���� ���
    BOOL           m_bDrag;               // ����ڰ� ��ũ���� �巡���ϰ� �ִ��� ����
    BOOL           m_bRightHanded;    // RH ��ǥ�� ��� ����

    D3DXVECTOR3 ScreenToVector( int sx, int sy );

public:
    LRESULT     HandleMouseMessages( HWND, UINT, WPARAM, LPARAM );

    D3DXMATRIX* GetRotationMatrix()         { return &m_matRotation; }
    D3DXMATRIX* GetRotationDeltaMatrix()    { return &m_matRotationDelta; }
    D3DXMATRIX* GetTranslationMatrix()      { return &m_matTranslation; }
    D3DXMATRIX* GetTranslationDeltaMatrix() { return &m_matTranslationDelta; }
    BOOL        IsBeingDragged()            { return m_bDrag; }

    VOID        SetRadius( FLOAT fRadius );
    VOID        SetWindow( INT w, INT h, FLOAT r=0.9 );
    VOID        SetRightHanded( BOOL bRightHanded ) { m_bRightHanded = bRightHanded; }

                CD3DArcBall();
    VOID        Init();
};

class CD3DCamera
{
    D3DXVECTOR3 m_vEyePt;    // �� ��Ʈ������ �Ӽ�
    D3DXVECTOR3 m_vLookatPt;
    D3DXVECTOR3 m_vUpVec;

    D3DXVECTOR3 m_vView;
    D3DXVECTOR3 m_vCross;

    D3DXMATRIX  m_matView;
    D3DXMATRIX  m_matBillboard; // ������ ȿ���� ���� Ư�� ��Ʈ����

    FLOAT       m_fFOV;    // ���� ����� �Ӽ�
    FLOAT       m_fAspect;
    FLOAT       m_fNearPlane;
    FLOAT       m_fFarPlane;
    D3DXMATRIX  m_matProj;

public:
    // Access functions
    D3DXVECTOR3 GetEyePt()           { return m_vEyePt; }
    D3DXVECTOR3 GetLookatPt()        { return m_vLookatPt; }
    D3DXVECTOR3 GetUpVec()           { return m_vUpVec; }
    D3DXVECTOR3 GetViewDir()         { return m_vView; }
    D3DXVECTOR3 GetCross()           { return m_vCross; }

    FLOAT       GetFOV()             { return m_fFOV; }
    FLOAT       GetAspect()          { return m_fAspect; }
    FLOAT       GetNearPlane()       { return m_fNearPlane; }
    FLOAT       GetFarPlane()        { return m_fFarPlane; }

    D3DXMATRIX  GetViewMatrix()      { return m_matView; }
    D3DXMATRIX  GetBillboardMatrix() { return m_matBillboard; }
    D3DXMATRIX  GetProjMatrix()      { return m_matProj; }

    VOID SetViewParams( D3DXVECTOR3 &vEyePt, D3DXVECTOR3& vLookatPt,
                        D3DXVECTOR3& vUpVec );
    VOID SetProjParams( FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane,
                        FLOAT fFarPlane );

    CD3DCamera();
};
#endif // D3DUTIL_H
