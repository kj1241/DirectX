#ifndef D3DAPP_H
#define D3DAPP_H

//�����ڵ�
enum APPMSGTYPE { MSG_NONE, MSGERR_APPMUSTEXIT, MSGWARN_SWITCHEDTOREF };

#define D3DAPPERR_NODIRECT3D          0x82000001
#define D3DAPPERR_NOWINDOW            0x82000002
#define D3DAPPERR_NOCOMPATIBLEDEVICES 0x82000003
#define D3DAPPERR_NOWINDOWABLEDEVICES 0x82000004
#define D3DAPPERR_NOHARDWAREDEVICE    0x82000005
#define D3DAPPERR_HALNOTCOMPATIBLE    0x82000006
#define D3DAPPERR_NOWINDOWEDHAL       0x82000007
#define D3DAPPERR_NODESKTOPHAL        0x82000008
#define D3DAPPERR_NOHALTHISMODE       0x82000009
#define D3DAPPERR_NONZEROREFCOUNT     0x8200000a
#define D3DAPPERR_MEDIANOTFOUND       0x8200000b
#define D3DAPPERR_RESETFAILED         0x8200000c
#define D3DAPPERR_NULLREFDEVICE       0x8200000d

class CD3DApplication
{
protected:
    CD3DEnumeration   m_d3dEnumeration;
    CD3DSettings      m_d3dSettings;

    // Internal variables for the state of the app
    bool              m_bWindowed;
    bool              m_bActive;
    bool              m_bDeviceLost;
    bool              m_bMinimized;
    bool              m_bMaximized;
    bool              m_bIgnoreSizeChange;
    bool              m_bDeviceObjectsInited;
    bool              m_bDeviceObjectsRestored;

    // Ÿ�ֿ̹� ���Ǵ� ���� ����
    bool              m_bFrameMoving;
    bool              m_bSingleStep;

    // ���� ���� ó�� �Լ�
    HRESULT DisplayErrorMsg( HRESULT hr, DWORD dwType );

    // 3D ����� �����ϰ� �������ϴ� ���� �Լ�
    static bool ConfirmDeviceHelper( D3DCAPS9* pCaps, 
        VertexProcessingType vertexProcessingType, D3DFORMAT backBufferFormat );
    void    BuildPresentParamsFromSettings();
    bool    FindBestWindowedMode( bool bRequireHAL, bool bRequireREF );
    bool    FindBestFullscreenMode( bool bRequireHAL, bool bRequireREF );
    HRESULT ChooseInitialD3DSettings();
    HRESULT Initialize3DEnvironment();
    HRESULT HandlePossibleSizeChange();
    HRESULT Reset3DEnvironment();
    HRESULT ToggleFullscreen();
    HRESULT ForceWindowed();
    HRESULT UserSelectNewDevice();
    void    Cleanup3DEnvironment();
    HRESULT Render3DEnvironment();
    virtual HRESULT AdjustWindowForChange();
    virtual void UpdateStats();

protected:
    // 3D ��� ���� �� �������� ���Ǵ� �ֿ� ��ü
    D3DPRESENT_PARAMETERS m_d3dpp;         // CreateDevice/Reset�� ���� �Ű�����
    HWND              m_hWnd;              // ���� �� â
    HWND              m_hWndFocus;         // D3D ��Ŀ�� â(���� m_hWnd�� ����)
    HMENU             m_hMenu;             // �� �޴� ǥ����(��ü ȭ���� �� ���⿡ �����)
    LPDIRECT3D9       m_pD3D;              // ���� D3D ��ü
    LPDIRECT3DDEVICE9 m_pd3dDevice;        // D3D ������ ��ġ
    D3DCAPS9          m_d3dCaps;           // ��ġ�� ĸ
    D3DSURFACE_DESC   m_d3dsdBackBuffer;   // ������� ǥ�� ����
    DWORD             m_dwCreateFlags;     // sw �Ǵ� hw ���� ó���� ��Ÿ���ϴ�.
    DWORD             m_dwWindowStyle;     // ��� ����ġ�� ���� ����� â ��Ÿ��
    RECT              m_rcWindowBounds;    // ��� ����ġ�� ���� ����� â ���
    RECT              m_rcWindowClient;    // ��� ��ȯ�� ���� ����� Ŭ���̾�Ʈ ���� ũ��

    // Ÿ�̹� ����
    FLOAT             m_fTime;             // ���� �ð�(��)
    FLOAT             m_fElapsedTime;      // ������ ������ ���� ����� �ð�
    FLOAT             m_fFPS;              // ���� ������ �ӵ�
    TCHAR             m_strDeviceStats[90];// D3D ��ġ ��踦 ��� ���ڿ�
    TCHAR             m_strFrameStats[90]; // ������ ��踦 ��� ���ڿ�

    // ���� ������ ������ ����
    TCHAR*            m_strWindowTitle;    // �� â ����
    DWORD             m_dwCreationWidth;   // â�� ����� �� ���Ǵ� �ʺ�
    DWORD             m_dwCreationHeight;  // ������ ������ ���� ����
    bool              m_bShowCursorWhenFullscreen; // ��ü ȭ���� �� Ŀ���� ǥ������ ����
    bool              m_bClipCursorWhenFullscreen; // ��ü ȭ���� �� Ŀ�� ��ġ�� �������� ����
    bool              m_bStartFullscreen;    // ��ü ȭ�� ��忡�� ���� �������� ����

    // �ۿ��� ������ 3D ��鿡 ���� ������ ������ �Լ�
    virtual HRESULT ConfirmDevice(D3DCAPS9*,DWORD,D3DFORMAT)   { return S_OK; }
    virtual HRESULT OneTimeSceneInit()                         { return S_OK; }
    virtual HRESULT InitDeviceObjects()                        { return S_OK; }
    virtual HRESULT RestoreDeviceObjects()                     { return S_OK; }
    virtual HRESULT FrameMove()                                { return S_OK; }
    virtual HRESULT Render()                                   { return S_OK; }
    virtual HRESULT InvalidateDeviceObjects()                  { return S_OK; }
    virtual HRESULT DeleteDeviceObjects()                      { return S_OK; }
    virtual HRESULT FinalCleanup()                             { return S_OK; }

public:
    // ���ø����̼��� ����, ����, �Ͻ�����, �����ϴ� �Լ�
    virtual HRESULT Create( HINSTANCE hInstance );
    virtual INT     Run();
    virtual LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual void    Pause( bool bPause );
    virtual ~CD3DApplication(){ }

    // ���� ������
    CD3DApplication();
};
#endif