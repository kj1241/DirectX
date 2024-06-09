#ifndef D3DENUM_H
#define D3DENUM_H

enum VertexProcessingType
{
    SOFTWARE_VP,
    MIXED_VP,
    HARDWARE_VP,
    PURE_HARDWARE_VP
};

struct D3DAdapterInfo
{
    int AdapterOrdinal;
    D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
    CArrayList* pDisplayModeList; // D3DDISPLAYMODE ���
    CArrayList* pDeviceInfoList; // D3DDeviceInfo ������ ���
    ~D3DAdapterInfo( void );
};

struct D3DDeviceInfo
{
    int AdapterOrdinal;
    D3DDEVTYPE DevType;
    D3DCAPS9 Caps;
    CArrayList* pDeviceComboList; // D3DDeviceCombo ������ ���
    ~D3DDeviceInfo( void );
};

struct D3DDSMSConflict
{
    D3DFORMAT DSFormat;
    D3DMULTISAMPLE_TYPE MSType;
};

struct D3DDeviceCombo
{
    int AdapterOrdinal;
    D3DDEVTYPE DevType;
    D3DFORMAT AdapterFormat;
    D3DFORMAT BackBufferFormat;
    bool IsWindowed;
    CArrayList* pDepthStencilFormatList; // D3DFORMAT ���
    CArrayList* pMultiSampleTypeList;    // D3DMULTISAMPLE_TYPE ���
    CArrayList* pMultiSampleQualityList; // DWORD ���(�� ���� ���� ������ ǰ�� ���� ��)
    CArrayList* pDSMSConflictList; // D3DDSMS�浹 ���
    CArrayList* pVertexProcessingTypeList; // VertexProcessingType ���
    CArrayList* pPresentIntervalList;   // D3DPRESENT_INTERVAL ���

    ~D3DDeviceCombo( void );
};


typedef bool(* CONFIRMDEVICECALLBACK)( D3DCAPS9* pCaps, 
    VertexProcessingType vertexProcessingType, D3DFORMAT backBufferFormat );

class CD3DEnumeration
{
private:
    IDirect3D9* m_pD3D;

private:
    HRESULT EnumerateDevices( D3DAdapterInfo* pAdapterInfo, CArrayList* pAdapterFormatList );
    HRESULT EnumerateDeviceCombos( D3DDeviceInfo* pDeviceInfo, CArrayList* pAdapterFormatList );
    void BuildDepthStencilFormatList( D3DDeviceCombo* pDeviceCombo );
    void BuildMultiSampleTypeList( D3DDeviceCombo* pDeviceCombo );
    void BuildDSMSConflictList( D3DDeviceCombo* pDeviceCombo );
    void BuildVertexProcessingTypeList( D3DDeviceInfo* pDeviceInfo, D3DDeviceCombo* pDeviceCombo );
    void BuildPresentIntervalList( D3DDeviceInfo* pDeviceInfo, D3DDeviceCombo* pDeviceCombo );

public:
    CArrayList* m_pAdapterInfoList;
    // ���� ������ ����Ͽ� ���, ����, 
    // ���� ���ŵ˴ϴ�.  ��ȭ�ϱ� ���� ���ϴ� ������ �����Ͻʽÿ�.
    // Enumerate().
    CONFIRMDEVICECALLBACK ConfirmDeviceCallback;
    UINT AppMinFullscreenWidth;
    UINT AppMinFullscreenHeight;
    UINT AppMinColorChannelBits; // ����� ������ ä�δ� �ּ� ���� ��Ʈ
    UINT AppMinAlphaChannelBits; // �� ���� ������ �ȼ��� �ּ� ���� ��Ʈ
    UINT AppMinDepthBits;
    UINT AppMinStencilBits;
    bool AppUsesDepthBuffer;
    bool AppUsesMixedVP; // ���� ȥ�� vp ��带 Ȱ���� �� �ִ��� ����
    bool AppRequiresWindowed;
    bool AppRequiresFullscreen;
    CArrayList* m_pAllowedAdapterFormatList;   // D3DFORMAT ���

    CD3DEnumeration();
    ~CD3DEnumeration();
    void SetD3D(IDirect3D9* pD3D) { m_pD3D = pD3D; }
    HRESULT Enumerate();
};
#endif