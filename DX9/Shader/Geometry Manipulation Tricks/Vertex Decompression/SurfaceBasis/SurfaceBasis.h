#pragma once

#define DXAPP_KEY        TEXT("Software\\DirectX9\\SurfaceBasis")

// ���� �Է� ���¸� �����ϴ� ����ü
struct UserInput
{
    BYTE diks[256];   // DirectInput Ű���� ���� ����

    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;

	BOOL b1,b2,b3,b4,b5,b6,b7,b8,b9;
	BOOL bf5,bf6,bf7,bf8,bf9;
};

enum DISPLAY_METHOD
{
	DM_CONTROL,			// ���� ������� �޽� �����
	DM_CONTROL_NPATCH,	// D3DX�� ����Ͽ� CPU���� N-��ġ�� �����մϴ�.
	DM_BARYCENTRIC, 	// '�Ϲ�' �޽ø� �������ϱ� ���� ���� �߽� ��ǥ�� ����߽��ϴ�.
	DM_NPATCH,			// ���� ���̴� N-��ġ
	DM_LINEAR,
};

class CMyD3DApplication : public CD3DApplication
{
    BOOL                    m_bLoadingApp;			// TRUE, ���� �ε� ���� ���
    CD3DFont*               m_pFont;				// �ؽ�Ʈ�� �׸��� �۲�
    ID3DXMesh*              m_pD3DXMesh;			// �������ڸ� �����ϴ� D3DX �޽�
    ID3DXPatchMesh*         m_pD3DXPatchMesh;		// �������ڸ� �����ϱ� ���� D3DX ��ġ �޽�
    ID3DXMesh*				m_pD3DXPatchMeshDest;	// �������ڸ� �����ϱ� ���� D3DX ��ġ �޽�
    LPDIRECTINPUT8          m_pDI;					// DirectInput ��ü
	LPDIRECTINPUTDEVICE8    m_pKeyboard;            // DirectInput Ű���� ��ġ
    UserInput               m_UserInput;			// ����� �Է��� �����ϴ� ����ü

    FLOAT                   m_fWorldRotX;			// ���� ȸ�� ���� X��
    FLOAT                   m_fWorldRotY;           // ���� ȸ�� ���� Y��

	LPD3DXEFFECT			m_controlFX;			// �Ϲ� ��ȯ ȿ��
	LPD3DXEFFECT			m_baryFX;				// ù ��° ���� �߽� ȿ��
	LPD3DXEFFECT			m_npatchFX;				// n��ġ ȿ��
	LPD3DXEFFECT			m_linearFX;				// n��ġ ȿ��

	unsigned int			m_numVertices;
	unsigned int			m_numIndices;
	float*					m_posVertexData;
	float*					m_normVertexData;
	WORD*					m_indexData;

	LPDIRECT3DVERTEXBUFFER9	m_pVB;					// �������� ���� VB
	LPDIRECT3DVERTEXBUFFER9	m_pIB;					// �������� ���� VB(���� ���)

	D3DXMATRIX				m_matProj;
	D3DXMATRIX				m_matView;
	D3DXMATRIX				m_matWorld;
	DISPLAY_METHOD			m_displayMethod;

	HRESULT SetupDisplayMethod();
	void CleanDisplayMethod();
	HRESULT GetXMeshData( LPD3DXMESH in );

	// �׼������� ���� ����
	unsigned int			m_lod;

	// ù ��° Bary ǥ�� �⺻ ����
	LPDIRECT3DVERTEXDECLARATION9	m_pBaryDecl;
	float*							m_baryPos;
	HRESULT							GenerateBaryConstants();

	// npatch ǥ�� ���
	LPDIRECT3DVERTEXDECLARATION9	m_pNPatchDecl;
	float*							m_npatchPos;
	float*							m_npatchNorm;
	HRESULT							GenerateNPatchConstants();
	HRESULT							GenerateNPatchBuffers( unsigned int LOD );
	HRESULT							GenerateD3DNPatchMesh( unsigned int LOD );

	// ���� ǥ�� ���
	LPDIRECT3DVERTEXDECLARATION9	m_pLinearDecl;
	float*							m_linearPos;
	float*							m_linearNorm;
	HRESULT							GenerateLinearConstants();
	HRESULT							GenerateLinearBuffers( unsigned int LOD );

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

    HRESULT RenderText();

    HRESULT InitInput( HWND hWnd );
    void    UpdateInput( UserInput* pUserInput );
    void    CleanupDirectInput();
    VOID    ReadSettings();
    VOID    WriteSettings();

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

