#ifndef AFX_QUANTISATION_H__C429C6DE_1BFE_4EAD_8122_272A65AAA8C3__INCLUDED_
#define AFX_QUANTISATION_H__C429C6DE_1BFE_4EAD_8122_272A65AAA8C3__INCLUDED_
#include "CompressedMesh.h"

#define DXAPP_KEY        TEXT("Software\\DirectX9\\SurfaceBasis")

// ���� �Է� ���¸� �����ϴ� ����ü
struct UserInput
{
	BYTE diks[256];   // DirectInput Ű���� ���� ���� 

	BOOL bRotateUp;
	BOOL bRotateDown;
	BOOL bRotateLeft;
	BOOL bRotateRight;
	BOOL bChange;

	BOOL b1, b2, b3, b4, b5, b6, b7, b8, b9;

	BOOL bf5, bf6, bf7, bf8, bf9;
};

//���ø����̼� Ŭ�����Դϴ�. �⺻ Ŭ������ CD3DApplication�� 
//��� Direct3D ���ÿ��� �ʿ��� �Ϲ� ����� �����մϴ�. 
//CMyD3DApplication�� �� ���� ���α׷��� Ư���� ����� �߰��մϴ�.
class CMyD3DApplication : public CD3DApplication
{
	enum COMPRESS_STATE
	{
		DISPLAY_QUANT_NORMAL = 0,
		DISPLAY_SO8BIT_POS,
		DISPLAY_CT8BIT_POS,
		DISPLAY_SO16BIT_POS,
		DISPLAY_CT16BIT_POS,
		DISPLAY_SCT16BIT_POS,
		DISPLAY_CT10BIT_POS,
		DISPLAY_CT101012BIT_POS,
	};

	enum MESH_NAME
	{
		DISPLAY_TEAPOT = 0,
		DISPLAY_CUBE,
		DISPLAY_TEAPOT_ROW,
		DISPLAY_CUBE_ROW,

		MAX_MESHES
	};

    BOOL                    m_bLoadingApp;          // ���� �ε��ϴ� ������ ����
    CD3DFont* m_pFont;                // �ؽ�Ʈ�� �׸��� �� ���Ǵ� ��Ʈ
    ID3DXMesh* m_pD3DXMesh[MAX_MESHES]; // Teapot�� �����ϴ� D3DX �޽�
    CompressedMesh* m_pQNMesh[MAX_MESHES];    // ����ȭ�� ����ȭ�� ���� �޽�
    CompressedMesh* m_pSO8Mesh[MAX_MESHES];    // ������ �� ������ 8��Ʈ ���� �޽�
    CompressedMesh* m_pCT8Mesh[MAX_MESHES];    // ���� ��ȯ 8��Ʈ ���� �޽�
    CompressedMesh* m_pSO16Mesh[MAX_MESHES];    // ������ �� ������ 8��Ʈ ���� �޽�
    CompressedMesh* m_pCT16Mesh[MAX_MESHES];    // ���� ��ȯ 16��Ʈ ���� �޽�
    CompressedMesh* m_pSCT16Mesh[MAX_MESHES];    // �����̵� ���� ��ȯ 16��Ʈ ���� �޽�
    CompressedMesh* m_pCT10Mesh[MAX_MESHES];    // ���� ��ȯ UDEC3N ��Ʈ ���� �޽�
    CompressedMesh* m_pCT101012Mesh[MAX_MESHES];    // ���� ��ȯ 10,10,12 ��Ʈ ���� �޽�

    IDirect3DVertexShader9* m_dwControlShader[MAX_MESHES];    // D3DX �޽� ���̴�
    IDirect3DVertexShader9* m_dwQNShader[MAX_MESHES];    // ����ȭ�� ����ȭ�� ���� �޽� ���̴�
    IDirect3DVertexShader9* m_dwSO8Shader[MAX_MESHES];    // ������ �� ������ 8��Ʈ ���� �޽� ���̴�
    IDirect3DVertexShader9* m_dwCT8Shader[MAX_MESHES];    // ���� ��ȯ 8��Ʈ ���� �޽� ���̴�
    IDirect3DVertexShader9* m_dwSO16Shader[MAX_MESHES];    // ������ �� ������ 8��Ʈ ���� �޽� ���̴�
    IDirect3DVertexShader9* m_dwCT16Shader[MAX_MESHES];    // ���� ��ȯ 16��Ʈ ���� �޽� ���̴�
    IDirect3DVertexShader9* m_dwSCT16Shader[MAX_MESHES]; // �����̵� ���� ��ȯ 16��Ʈ ���� �޽� ���̴�
    IDirect3DVertexShader9* m_dwCT10Shader[MAX_MESHES]; //���� ��ȯ DEC3N ���� �޽� ���̴�  
    IDirect3DVertexShader9* m_dwCT101012Shader[MAX_MESHES]; //���� ��ȯ 10,10,12 ��Ʈ ���� �޽� ���̴�

    bool                    m_original;             // � �޽ð� ����ȭ�Ǿ� �ִ���?
    D3DXMATRIX              m_matView;              // �� ���
    D3DXMATRIX              m_matProj;              // ���� ���
    D3DXMATRIX              m_matWorld;             // ���� ���

    LPDIRECTINPUT8          m_pDI;                  // DirectInput ��ü
    LPDIRECTINPUTDEVICE8    m_pKeyboard;            // DirectInput Ű���� ��ġ
    UserInput               m_UserInput;            // ����� �Է��� �����ϴ� ����ü 

    FLOAT                   m_fWorldRotX;           // ���� ȸ�� ���� X��
    FLOAT                   m_fWorldRotY;           // ���� ȸ�� ���� Y��
    CompressedMesh*         m_pCompressMesh;        // ���� ���� ���� �޽� (����)
    COMPRESS_STATE          m_compressState;        // ���� ����ȭ�� ����
	MESH_NAME				m_meshState;			// ���� ���̴� �޽�

protected:
    HRESULT OneTimeSceneInit();
    HRESULT InitDeviceObjects();
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();
    HRESULT Render();
    HRESULT FrameMove();
    HRESULT FinalCleanup();
    HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

    HRESULT RenderText();

    HRESULT InitInput( HWND hWnd );
    void    UpdateInput( UserInput* pUserInput );
    void    CleanupDirectInput();

    VOID    ReadSettings();
    VOID    WriteSettings();

	HRESULT GenerateTeapotRow( ID3DXMesh** mesh );
	HRESULT GenerateCubeRow( ID3DXMesh** mesh );
	HRESULT CreateVertexShaders( unsigned int i);

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
};
#endif // !defined(AFX_QUANTISATION_H__C429C6DE_1BFE_4EAD_8122_272A65AAA8C3__INCLUDED_)