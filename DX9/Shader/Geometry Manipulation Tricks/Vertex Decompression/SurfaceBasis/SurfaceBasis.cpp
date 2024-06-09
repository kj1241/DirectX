#define STRICT
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr.h>
#include <tchar.h>
#include <dinput.h>
#include "../Framework/DXUtil.h"
#include "../Framework/D3DEnumeration.h"
#include "../Framework/D3DSettings.h"
#include "../Framework/D3DApp.h"
#include "../Framework/D3DFont.h"
#include "../Framework/D3DFile.h"
#include "../Framework/D3DUtil.h"
#include "../Framework/resource.h"
#include "SurfaceBasis.h"
#include <stack>

CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;

// �̷� ���� ���� �߽� ��ǥ�� ����ȭ�ǰ� ���� ������ �߻��մϴ�.
const bool bFloatNPatch = true;

enum BarycentricPrecision
{
	BYTE_BARYCENTRIC,
	WORD_BARYCENTRIC,
	FLOAT_BARYCENTRIC,
};
const enum BarycentricPrecision gBarycentricPrecision = FLOAT_BARYCENTRIC;

INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}

CMyD3DApplication::CMyD3DApplication()
{
    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "SurfaceBasis" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
    m_pD3DXMesh                 = NULL;
	m_pD3DXPatchMesh			= NULL;
	m_pD3DXPatchMeshDest		= NULL;
    m_pDI                       = NULL;
    m_pKeyboard                 = NULL;

	m_pBaryDecl					= NULL;
	m_pNPatchDecl				= NULL;
	m_pLinearDecl				= NULL;

	m_pVB						= NULL; 
	
	m_baryFX					= NULL;
	m_npatchFX					= NULL;
	
	m_posVertexData				= NULL;
	m_normVertexData			= NULL;
	m_indexData					= NULL;

	m_numVertices				= 0;
	m_numIndices				= 0;

	m_baryPos					= NULL;
	m_npatchPos					= NULL;
	m_npatchNorm				= NULL;

	m_linearPos					= NULL;
	m_linearNorm				= NULL;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = 0.0f;

	m_displayMethod =			DM_BARYCENTRIC;

    // ������Ʈ������ ���� �б�
    ReadSettings();
}

CMyD3DApplication::~CMyD3DApplication()
{
}

//FinalCleanup()�� ���� �̷�ϴ�.
// â�� �����Ǿ��� IDirect3D9 �������̽��� �����Ǿ����ϴ�.
// �����Ǿ����� ���� ��ġ�� �������� �ʾҽ��ϴ�.  ���⿡�� �� �� �ֽ��ϴ�
// ���ø����̼� ���� �ʱ�ȭ �� ������ �����մϴ�.
// ��ġ�� �������� �ʽ��ϴ�.
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
	// TODO: ��ȸ�� �ʱ�ȭ ����
	// �� �ε��� �Ϸ�� ������ �ε� ���� �޽��� �׸���
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );
	// DirectInput �ʱ�ȭ
    InitInput( m_hWnd );

    m_bLoadingApp = FALSE;
    return S_OK;
}

//������Ʈ������ �� ���� �б�
VOID CMyD3DApplication::ReadSettings()
{
    HKEY hkey;
    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, DXAPP_KEY,  0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL ) )
    {
		// TODO: �ʿ信 ���� ����

		// ����� â �ʺ�/���̸� �н��ϴ�.  �̰��� ���� ���� ���̸�,
		// DXUtil_Read*() �Լ��� ����ϴ� ����Դϴ�.
        DXUtil_ReadIntRegKey( hkey, TEXT("Width"), &m_dwCreationWidth, m_dwCreationWidth );
        DXUtil_ReadIntRegKey( hkey, TEXT("Height"), &m_dwCreationHeight, m_dwCreationHeight );

        RegCloseKey( hkey );
    }
}

//������Ʈ���� �� ���� ����
VOID CMyD3DApplication::WriteSettings()
{
    HKEY hkey;

    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, DXAPP_KEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL ) )
    {
		// TODO: �ʿ信 ���� ����
		
		// â �ʺ�/���̸� ���ϴ�.  �̰��� ���� ���� ���̸�, DXUtil_Write*() �Լ��� ����ϴ� ����Դϴ�.
        DXUtil_WriteIntRegKey( hkey, TEXT("Width"), m_rcWindowClient.right );
        DXUtil_WriteIntRegKey( hkey, TEXT("Height"), m_rcWindowClient.bottom );

        RegCloseKey( hkey );
    }
}

//DirectInput ��ü �ʱ�ȭ
HRESULT CMyD3DApplication::InitInput( HWND hWnd )
{
    HRESULT hr;

	// IDirectInput8* ����
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&m_pDI, NULL ) ) )
        return DXTRACE_ERR( "DirectInput8Create", hr );
    
	// Ű����� IDirectInputDevice8*�� �����մϴ�.
    if( FAILED( hr = m_pDI->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL ) ) )
        return DXTRACE_ERR( "CreateDevice", hr );
    
	// Ű���� ������ ������ �����մϴ�.
    if( FAILED( hr = m_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
        return DXTRACE_ERR( "SetDataFormat", hr );
    
	// Ű���忡�� ���� ������ �����մϴ�.
    if( FAILED( hr = m_pKeyboard->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND |  DISCL_NOWINKEY ) ) )
        return DXTRACE_ERR( "SetCooperativeLevel", hr );

	// Ű���� ȹ��
    m_pKeyboard->Acquire();

    return S_OK;
}

// ��� �ʱ�ȭ �߿� ȣ��Ǵ� �� �ڵ�� ���÷��� ��⸦ Ȯ���մϴ�. �ּ����� ��� ��Ʈ�� ����
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    BOOL bCapsAcceptable;

	// TODO: �̷��� ���÷��� ĸ�� ���Ǵ��� Ȯ���ϱ� ���� �˻縦 �����մϴ�.
    bCapsAcceptable = TRUE;

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}

// DeleteDeviceObjects()�� ����
// ��ġ�� �����Ǿ����ϴ�.  �սǵ��� �ʴ� ���ҽ� ���⿡�� Reset()�� ������ �� �ֽ��ϴ�. 
// -- D3DPOOL_MANAGED�� ���ҽ�, D3DPOOL_SCRATCH �Ǵ� D3DPOOL_SYSTEMMEM.  
// ������ ���� ������ �̹��� ǥ�� CreateImageSurface�� �սǵ��� ������ ���⿡�� ������ �� �ֽ��ϴ�.  
// ���ؽ� ���̴��� �ȼ� ���̴��� ���⿡�� ������ ���� �ֽ��ϴ�.
// Reset()���� �ս�
HRESULT CMyD3DApplication::InitDeviceObjects()
{
	// TODO: ��ġ ��ü ����

    HRESULT hr;

	// �۲� �ʱ�ȭ
    m_pFont->InitDeviceObjects( m_pd3dDevice );

	// D3DX�� ����Ͽ� ť�� �޽� ����
    if( FAILED( hr = D3DXCreateBox( m_pd3dDevice,  1, 1, 1, &m_pD3DXMesh, NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateTeapot", hr );

	D3DXWELDEPSILONS Epsilons;
	memset( &Epsilons, 0, sizeof(D3DXWELDEPSILONS) );
	Epsilons.Normal = 100000;
	D3DXWeldVertices( m_pD3DXMesh,  0, &Epsilons, 0, 0, 0, 0 );

	// ��� ������ �����մϴ�(���� n-��ġ�� �� ��̷ӽ��ϴ�).
	D3DXComputeNormals(m_pD3DXMesh, 0 );

	// ���� n-��ġ�� ���� ��ġ �޽ø� �����մϴ�.
    if( FAILED( hr = D3DXCreateNPatchMesh( m_pD3DXMesh, &m_pD3DXPatchMesh ) ) )
        return DXTRACE_ERR( "D3DXCreateNPatchMesh", hr );

	// �׼����̼� �ӵ� ���
	if( FAILED( hr = m_pD3DXPatchMesh->Optimize( 0 ) ) )
        return DXTRACE_ERR( "m_pD3DXPatchMesh->Optimize", hr );

	if( FAILED( hr = GenerateD3DNPatchMesh( 1 ) ) )
        return DXTRACE_ERR( "GenerateD3DNPatchMesh", hr );

	GetXMeshData( m_pD3DXMesh );

	// �⺻ ���� �߽� ��ǥ
	if( FAILED( hr = D3DXCreateEffectFromFile( m_pd3dDevice, "control.fx", 0, 0, 0, 0, &m_controlFX, 0 ) ) )
		return DXTRACE_ERR( "D3DXCreateEffectFromFile", hr );

	// �⺻ ���� �߽� ��ǥ
	if( FAILED( hr = D3DXCreateEffectFromFile( m_pd3dDevice, "bary.fx", 0, 0, 0, 0, &m_baryFX, 0 ) ) )
		return DXTRACE_ERR( "D3DXCreateEffectFromFile", hr );

	// n��ġ
	if( FAILED( hr = D3DXCreateEffectFromFile( m_pd3dDevice, "npatch.fx", 0, 0, 0, 0, &m_npatchFX, 0 ) ) )
		return DXTRACE_ERR( "D3DXCreateEffectFromFile", hr );

	// n��ġ
	if( FAILED( hr = D3DXCreateEffectFromFile( m_pd3dDevice, "linear.fx", 0, 0, 0, 0, &m_linearFX, 0 ) ) )
		return DXTRACE_ERR( "D3DXCreateEffectFromFile", hr );

	// ���� �߽� ��ǥ ����� �⺻���Դϴ�.
	if( FAILED( hr = GenerateBaryConstants() ) )
		return DXTRACE_ERR( "GenerateBaryConstants", hr );
	
    return S_OK;
}

HRESULT CMyD3DApplication::GetXMeshData( LPD3DXMESH in )
{
	HRESULT hRes;

	// �Է� ���� �����͸� ����ϴ�.
	float* inStream = 0;
	hRes = in->LockVertexBuffer(D3DLOCK_READONLY, (void**)&inStream);
	if( FAILED(hRes) ) return hRes;

	m_numVertices = in->GetNumVertices();

	// ������ �����ϱ� ���� �Ϻ� �޸𸮸� �Ҵ��մϴ�.
	m_posVertexData = new float[ m_numVertices * 3 ];
	m_normVertexData = new float[ m_numVertices * 3 ];

	// ���� �����͸� �����մϴ�.
	for(unsigned int i=0;i < m_numVertices;i++)
	{
		m_posVertexData[(i*3)+0] = inStream[0];
		m_posVertexData[(i*3)+1] = inStream[1];
		m_posVertexData[(i*3)+2] = inStream[2];
		m_normVertexData[(i*3)+0] = inStream[3];
		m_normVertexData[(i*3)+1] = inStream[4];
		m_normVertexData[(i*3)+2] = inStream[5];

		// ���� ����
		inStream += 6;
	}
	
	hRes = in->UnlockVertexBuffer();
	if( FAILED(hRes) ) return hRes;

	// ���� �ε����� �����մϴ�.
	m_numIndices = in->GetNumFaces() * 3;

	// 16��Ʈ �ε�����
	const unsigned int ibsize = m_numIndices * sizeof(WORD);

	m_indexData = new WORD[ m_numIndices ];

	WORD* inIBStream = 0;
	hRes = in->LockIndexBuffer( D3DLOCK_READONLY, (void**)&inIBStream);
	if( FAILED(hRes) ) return hRes;

	memcpy(m_indexData, inIBStream, ibsize);

	// ���� �� �ε��� ���۸� ��� ��� �����մϴ�.
	hRes = in->UnlockIndexBuffer();
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}

HRESULT CMyD3DApplication::GenerateBaryConstants()
{
	HRESULT hRes;
	// ���� ���� �������� ������ �����մϴ�.
	m_baryPos = new float[ m_numIndices * 4];
	for(unsigned int i=0; i < m_numIndices;i++)
	{
		m_baryPos[ (i*4) + 0 ] = m_posVertexData[ (m_indexData[i]*3) + 0 ];
		m_baryPos[ (i*4) + 1 ] = m_posVertexData[ (m_indexData[i]*3) + 1 ];
		m_baryPos[ (i*4) + 2 ] = m_posVertexData[ (m_indexData[i]*3) + 2 ];
		m_baryPos[ (i*4) + 3 ] = 0;
	}

	const unsigned int vbSize = sizeof(unsigned int) * m_numIndices;

	hRes = m_pd3dDevice->CreateVertexBuffer( vbSize, 0, 0,D3DPOOL_MANAGED, &m_pVB, NULL );
	if( FAILED(hRes) ) return hRes;

	unsigned int* outStream = 0;
	hRes = m_pVB->Lock(0,0, (void**)&outStream, 0 );
	if( FAILED(hRes) ) return hRes;

	// ����(����ȭ�� ���� �߽�) ��ǥ�� ����մϴ�.
	for( unsigned int index=0; index < m_numIndices/3;index++)
	{
		// ù ��° ���� : i=1, j=k=0
		// �� ��° ���� : j=1, i=k=0
		// �� ��° ���� : k=1, i=j=0 (��, k�� ������� ����)

		unsigned int packVert;
		unsigned int i,j,k;

		i = 255; j = k = 0; // ù ��° ����
		packVert = (i << 16) + (j << 8) + (index << 0); // D3DCOLOR ���� Ǯ�⿡ ����
		outStream[ (index*3) + 0 ] = packVert;
		j = 255; i = k = 0; // �� ��° ����
		packVert = (i << 16) + (j << 8) + (index << 0); // D3DCOLOR ���� Ǯ�⿡ ����
		outStream[ (index*3) + 1 ] = packVert;
		k = 255; i = j = 0; // �� ��° ����
		packVert = (i << 16) + (j << 8) + (index << 0); // D3DCOLOR ���� Ǯ�⿡ ����
		outStream[ (index*3) + 2 ] = packVert;
	}

	// ���� ���� ������ ����� �����մϴ�.
	hRes = m_pVB->Unlock();
	if( FAILED(hRes) ) return hRes;

	D3DVERTEXELEMENT9	Decl[MAX_FVF_DECL_SIZE];
	// ���� D3D ����(���� ��� ��������)
	Decl[0].Stream		= 0;
	Decl[0].Offset		= 0;
	Decl[0].Type		= D3DDECLTYPE_D3DCOLOR;
	Decl[0].Method		= D3DDECLMETHOD_DEFAULT;
	Decl[0].Usage		= D3DDECLUSAGE_POSITION;
	Decl[0].UsageIndex	= 0;
	
	// D3DDECL_END()
	Decl[1].Stream		= 0xFF;
	Decl[1].Offset		= 0;
	Decl[1].Type		= D3DDECLTYPE_UNUSED;
	Decl[1].Method		= 0;
	Decl[1].Usage		= 0;
	Decl[1].UsageIndex	= 0;

	SAFE_RELEASE( m_pBaryDecl );
	hRes = m_pd3dDevice->CreateVertexDeclaration( Decl, &m_pBaryDecl );
	if( FAILED(hRes) ) return hRes;

	// ��� ������ �����մϴ�.
	m_baryFX->SetVectorArray( "VertexPos", (D3DXVECTOR4*)m_baryPos, m_numIndices );

	return S_OK;
}

HRESULT CMyD3DApplication::GenerateNPatchConstants()
{
	HRESULT hRes;
	// ���� ���� �������� ������ �����մϴ�.
	m_npatchPos = new float[ m_numIndices * 4];
	m_npatchNorm = new float[ m_numIndices * 4];

	for(unsigned int i=0; i < m_numIndices;i++)
	{
		m_npatchPos[ (i*4) + 0 ] = m_posVertexData[ (m_indexData[i]*3) + 0 ];
		m_npatchPos[ (i*4) + 1 ] = m_posVertexData[ (m_indexData[i]*3) + 1 ];
		m_npatchPos[ (i*4) + 2 ] = m_posVertexData[ (m_indexData[i]*3) + 2 ];
		m_npatchPos[ (i*4) + 3 ] = 0;

		m_npatchNorm[ (i*4) + 0 ] = m_normVertexData[ (m_indexData[i]*3) + 0 ];
		m_npatchNorm[ (i*4) + 1 ] = m_normVertexData[ (m_indexData[i]*3) + 1 ];
		m_npatchNorm[ (i*4) + 2 ] = m_normVertexData[ (m_indexData[i]*3) + 2 ];
		m_npatchNorm[ (i*4) + 3 ] = 0;
	}

	hRes = GenerateNPatchBuffers( m_lod );
	if( FAILED(hRes) ) return hRes;

	D3DVERTEXELEMENT9	Decl[MAX_FVF_DECL_SIZE];
	// ���� D3D ����
	Decl[0].Stream		= 0;
	Decl[0].Offset		= 0;
	if( bFloatNPatch == false )
		Decl[0].Type	= D3DDECLTYPE_D3DCOLOR;
	else
		Decl[0].Type	= D3DDECLTYPE_FLOAT3;
	Decl[0].Method		= D3DDECLMETHOD_DEFAULT;
	Decl[0].Usage		= D3DDECLUSAGE_POSITION;
	Decl[0].UsageIndex	= 0;
	
	// D3DDECL_END()
	Decl[1].Stream		= 0xFF;
	Decl[1].Offset		= 0;
	Decl[1].Type		= D3DDECLTYPE_UNUSED;
	Decl[1].Method		= 0;
	Decl[1].Usage		= 0;
	Decl[1].UsageIndex	= 0;

	SAFE_RELEASE( m_pNPatchDecl );
	hRes = m_pd3dDevice->CreateVertexDeclaration( Decl, &m_pNPatchDecl );
	if( FAILED(hRes) ) return hRes;

	// ��� ������ �����մϴ�.
	m_npatchFX->SetVectorArray( "VertexPos", (D3DXVECTOR4*)m_npatchPos, m_numIndices );
	m_npatchFX->SetVectorArray( "VertexNorm", (D3DXVECTOR4*)m_npatchNorm, m_numIndices );

	return S_OK;
}

HRESULT CMyD3DApplication::GenerateLinearConstants()
{
	HRESULT hRes;

	// ���� ���� �������� ������ �����մϴ�.
	m_linearPos = new float[ m_numVertices * 4];
	m_linearNorm = new float[ m_numVertices * 4];

	for(unsigned int i=0; i < m_numVertices;i++)
	{
		m_linearPos[ (i*4) + 0 ] = m_posVertexData[ (i*3) + 0 ];
		m_linearPos[ (i*4) + 1 ] = m_posVertexData[ (i*3) + 1 ];
		m_linearPos[ (i*4) + 2 ] = m_posVertexData[ (i*3) + 2 ];
		m_linearPos[ (i*4) + 3 ] = 0;

		m_linearNorm[ (i*4) + 0 ] = m_normVertexData[ (i*3) + 0 ];
		m_linearNorm[ (i*4) + 1 ] = m_normVertexData[ (i*3) + 1 ];
		m_linearNorm[ (i*4) + 2 ] = m_normVertexData[ (i*3) + 2 ];
		m_linearNorm[ (i*4) + 3 ] = 0;
	}

	// ���� �����͸� ���� ����մϴ�.
	hRes = GenerateLinearBuffers( m_lod );
	if( FAILED(hRes) ) return hRes;


	D3DVERTEXELEMENT9	Decl[MAX_FVF_DECL_SIZE];


	// ���� D3D ���� 
	// ���� �߽� ��ǥ
	Decl[0].Stream		= 0;
	Decl[0].Offset		= 0;
	if( gBarycentricPrecision == BYTE_BARYCENTRIC )
		Decl[0].Type		= D3DDECLTYPE_D3DCOLOR;
	else if(gBarycentricPrecision == WORD_BARYCENTRIC)
		Decl[0].Type		= D3DDECLTYPE_USHORT4N;
	else if(gBarycentricPrecision == FLOAT_BARYCENTRIC)
		Decl[0].Type		= D3DDECLTYPE_FLOAT2;

	Decl[0].Method		= D3DDECLMETHOD_DEFAULT;
	Decl[0].Usage		= D3DDECLUSAGE_POSITION;
	Decl[0].UsageIndex	= 0;

	// �ε���
	Decl[1].Stream		= 0;
	if(gBarycentricPrecision == FLOAT_BARYCENTRIC)
		Decl[1].Offset		= sizeof( float ) * 2;
	else
		Decl[1].Offset		= sizeof( unsigned int );
	Decl[1].Type		= D3DDECLTYPE_D3DCOLOR;
	Decl[1].Method		= D3DDECLMETHOD_DEFAULT;
	Decl[1].Usage		= D3DDECLUSAGE_POSITION;
	Decl[1].UsageIndex	= 1;

	// D3DDECL_END()
	Decl[2].Stream		= 0xFF;
	Decl[2].Offset		= 0;
	Decl[2].Type		= D3DDECLTYPE_UNUSED;
	Decl[2].Method		= 0;
	Decl[2].Usage		= 0;
	Decl[2].UsageIndex	= 0;

	SAFE_RELEASE( m_pLinearDecl );
	hRes = m_pd3dDevice->CreateVertexDeclaration( Decl, &m_pLinearDecl );
	if( FAILED(hRes) ) 
		return hRes;

	// ��� ������ �����մϴ�.
	m_linearFX->SetVectorArray( "VertexPos", (D3DXVECTOR4*)m_linearPos, m_numVertices );
	m_linearFX->SetVectorArray( "VertexNorm", (D3DXVECTOR4*)m_linearNorm, m_numVertices );

	return S_OK;
}

static unsigned int calcPatchVertsPerOrigTri( unsigned int n )
{
	return unsigned int(pow(4, n)) * 3;
}

//�����߽���ǥ
struct BARYCENTRIC_COORDS
{
	float i, j; // k�� ���� ����� �� �ֽ��ϴ�.

	BARYCENTRIC_COORDS() : i(0), j(0) {};
	BARYCENTRIC_COORDS( float x, float y ) : i(x), j(y) {};
	BARYCENTRIC_COORDS( const BARYCENTRIC_COORDS& right) : i(right.i), j(right.j) {};
	float getI() { return i; };
	float getJ() { return j; };
	float getK() { return 1.f - i - j; };

	BYTE getByteQuantisedI()
	{ 
		// �� �ڵ�� �ش����� ���е� �������� ���� ������ ���̴� �� ������ �˴ϴ�.
		unsigned int i = getI() * 256.f;
		if( i >= 256 )
			i = 255;
		return BYTE( i ); 
	};
	BYTE getByteQuantisedJ()
	{ 
		// �� �ڵ�� �ش����� ���е� �������� ���� ������ ���̴� �� ������ �˴ϴ�.
		unsigned int j = getJ() * 256.f;
		if( j >= 256 )
			j = 255;
		return BYTE( j ); 
	};
	BYTE getByteQuantisedK()
	{
		// �� �ڵ�� �ش����� ���е� �������� ���� ������ ���̴� �� ������ �˴ϴ�.
		unsigned int k = getK() * 256.f;
		if( k >= 256 )
			k = 255;
		return BYTE( k ); 
	};

	WORD getWordQuantisedI()
	{ 
		unsigned int i = getI() * 65535.f;
		return WORD( i ); 
	};
	WORD getWordQuantisedJ()
	{ 
		unsigned int j = getJ() * 65535.f;
		return WORD( j ); 
	};
	WORD getWordQuantisedK()
	{ 
		unsigned int k = getK() * 65535.f;
		return WORD( k ); 
	};

	BARYCENTRIC_COORDS& operator+=( const BARYCENTRIC_COORDS& right )
	{
		i = i + right.i;
		j = j + right.j;
		return *this;
	}
	BARYCENTRIC_COORDS operator+( const BARYCENTRIC_COORDS& right ) const
	{
		BARYCENTRIC_COORDS ret(*this);
		ret += right;
		return ret;
	}

	BARYCENTRIC_COORDS& operator/( float right )
	{
		i = i / right;
		j = j / right;
		return *this;
	}

};

struct BARYCENTRIC_TRIANGLE
{
	BARYCENTRIC_COORDS a, b, c;
	unsigned int lod;

	// �⺻ ǥ�� �ﰢ��
	BARYCENTRIC_TRIANGLE() : a(1,0), b(0,1), c(0,0), lod(0) {};

	BARYCENTRIC_TRIANGLE( const BARYCENTRIC_TRIANGLE& right )
	{
		a = right.a;
		b = right.b;
		c = right.c;
		lod = right.lod;
	}
};

static void PatchTesselate( BARYCENTRIC_TRIANGLE tri,BARYCENTRIC_TRIANGLE& a,BARYCENTRIC_TRIANGLE& b,BARYCENTRIC_TRIANGLE& c,BARYCENTRIC_TRIANGLE& d )
{
	BARYCENTRIC_COORDS ab = (tri.a + tri.b) / 2;
	BARYCENTRIC_COORDS bc = (tri.b + tri.c) / 2;
	BARYCENTRIC_COORDS ac = (tri.a + tri.c) / 2;

	a.a = tri.a;
	a.b = ab;
	a.c = ac;
	a.lod = tri.lod+1;

	b.a = bc;
	b.b = tri.c;
	b.c = ac;
	b.lod = tri.lod+1;

	c.a = bc;
	c.b = ab;
	c.c = tri.b;
	c.lod = tri.lod+1;

	d.a = bc;
	d.b = ac;
	d.c = ab;
	d.lod = tri.lod+1;
}

HRESULT CMyD3DApplication::GenerateD3DNPatchMesh( unsigned int LOD )
{
	HRESULT hr;
	LOD = LOD + 1; // D3D�� �� ����� ����

	if( LOD == m_lod && m_pD3DXPatchMeshDest != 0 )
		return S_OK;

	m_lod = LOD;
	SAFE_RELEASE( m_pD3DXPatchMeshDest );

	// �׼�����Ʈ�� �޽��� �����ϴ� �� �ʿ��� �޽��� ũ�⸦ �����մϴ�.
	DWORD NumTris, NumVerts;
	if( FAILED( hr = m_pD3DXPatchMesh->GetTessSize( (float)LOD, FALSE, &NumTris, &NumVerts ) ) )
        return DXTRACE_ERR( "m_pD3DXPatchMesh->Optimize", hr );

	if( FAILED( hr = D3DXCreateMeshFVF( NumTris, NumVerts, 0, m_pD3DXMesh->GetFVF(), m_pd3dDevice, &m_pD3DXPatchMeshDest ) ) )
        return DXTRACE_ERR( "m_pD3DXPatchMesh->Optimize", hr );

	if( FAILED( hr = m_pD3DXPatchMesh->Tessellate( (float)LOD, m_pD3DXPatchMeshDest ) ) )
        return DXTRACE_ERR( "m_pD3DXPatchMesh->Tessellate", hr );

	return S_OK;
}

//�� �ڵ�� Direct3D�� ����Ͽ� ���� ���� ������(LOD)�� ���� ��ġ�� �����ϰ� �̸� ���� ����(Vertex Buffer)�� �����ϴ� �� ���� �Լ��Դϴ�.
//�ϳ��� �Ϲ����� ���� ���۸� �����ϰ�, �ٸ� �ϳ��� ���� ���۸� �����մϴ�. 
//�� �Լ� ��� Ư�� ������ ������(LOD)�� ���� �ﰢ�� ��ġ�� ����(tessellate)�ϰ� �� ����� ���� ���ۿ� �����մϴ�.
//�ٸ���Ʈ�� ��ǥ�� ����Ͽ� �� ������ ǥ���մϴ�.
//NPatchBuffer ���� 
//���� ���۸� �����ϰ�, �̸� ������ LOD�� ���� tessellation�� ��ġ �����ͷ� ä��ϴ�.
HRESULT CMyD3DApplication::GenerateNPatchBuffers( unsigned int LOD )
{
	HRESULT hRes;

	m_lod = LOD;

	SAFE_RELEASE( m_pVB );

	const unsigned int numTri = m_numIndices / 3;
	const unsigned int numVerts = calcPatchVertsPerOrigTri( LOD ) * numTri;
	unsigned int vbSize;
	
	if( bFloatNPatch == false )
	{
		vbSize = sizeof(unsigned int) * numVerts;
	} else
	{
		vbSize = sizeof(float) * 3 * numVerts;
	}

	hRes = m_pd3dDevice->CreateVertexBuffer( vbSize, 0, 0,D3DPOOL_MANAGED, &m_pVB, NULL );
	if( FAILED(hRes) ) return hRes;

	unsigned int *outStream;

	hRes = m_pVB->Lock(0, vbSize, (void**)&outStream, 0 );
	if( FAILED(hRes) ) return hRes;

	for(unsigned int index=0;index < m_numIndices/3;index++)
	{
		std::stack<BARYCENTRIC_TRIANGLE> lodStack;
		lodStack.push( BARYCENTRIC_TRIANGLE() );

		while( !lodStack.empty() )
		{
			BARYCENTRIC_TRIANGLE tri = lodStack.top();
			lodStack.pop();

			if( tri.lod < LOD)
			{
				BARYCENTRIC_TRIANGLE tri_a, tri_b, tri_c, tri_d;

				PatchTesselate( tri, tri_a, tri_b, tri_c, tri_d );
				lodStack.push( tri_a );
				lodStack.push( tri_b );
				lodStack.push( tri_c );
				lodStack.push( tri_d );
			} else
			{
				if( tri.lod == LOD )
				{
					if( bFloatNPatch == false )
					{
						//��� ��ǥ
						*(outStream+0) = ((unsigned int)(tri.a.i*255)<<16) | ((unsigned int)(tri.a.j*255)<<8) | (index<<0); // correct for D3DCOLOR unpacking
						*(outStream+1) = ((unsigned int)(tri.b.i*255)<<16) | ((unsigned int)(tri.b.j*255)<<8) | (index<<0); // correct for D3DCOLOR unpacking
						*(outStream+2) = ((unsigned int)(tri.c.i*255)<<16) | ((unsigned int)(tri.c.j*255)<<8) | (index<<0); // correct for D3DCOLOR unpacking
						outStream += 3;
					} else
					{
						*((float*)outStream+0) = tri.a.i;
						*((float*)outStream+1) = tri.a.j;
						*((float*)outStream+2) = index / 255.f;
						*((float*)outStream+3) = tri.b.i;
						*((float*)outStream+4) = tri.b.j;
						*((float*)outStream+5) = index / 255.f;
						*((float*)outStream+6) = tri.c.i;
						*((float*)outStream+7) = tri.c.j;
						*((float*)outStream+8) = index / 255.f;
						outStream += 9;
					}
				}
			}
		}
	}

	// ���� ���� ���۸� ��� �����ϼ���.
	hRes = m_pVB->Unlock();
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}

//���� ���� ����
//���� ���۸� �����ϰ�, �̸� ������ LOD�� ���� tessellation�� ��ġ �����ͷ� ä��ϴ�.
HRESULT CMyD3DApplication::GenerateLinearBuffers( unsigned int LOD )
{
	HRESULT hRes;

	m_lod = LOD;

	SAFE_RELEASE( m_pVB );

	const unsigned int numTri = m_numIndices / 3;
	const unsigned int numVerts = calcPatchVertsPerOrigTri( LOD ) * numTri;
	unsigned int vbSize;
	
	// ���� �߽��� ��� ǥ�鿡�� ���� ����
	// ���� �ݾ��� ����ϹǷ� 2���� ��Ʈ������ �����ϸ� ���� ����� ������ �� �ֽ��ϴ�.
	// ��
	if( gBarycentricPrecision == FLOAT_BARYCENTRIC )
		vbSize = (sizeof(float)*2 + sizeof(unsigned int)) * numVerts;
	else
		vbSize = (sizeof(unsigned int) * 2) * numVerts;
	
	hRes = m_pd3dDevice->CreateVertexBuffer( vbSize, 0, 0,D3DPOOL_MANAGED, &m_pVB, NULL );
	if( FAILED(hRes) ) 
		return hRes;

	unsigned int *outStream;

	hRes = m_pVB->Lock(0, vbSize, (void**)&outStream, 0 );
	if( FAILED(hRes) ) 
		return hRes;

	for(unsigned int index=0;index < m_numIndices/3;index++)
	{
		std::stack<BARYCENTRIC_TRIANGLE> lodStack;
		lodStack.push( BARYCENTRIC_TRIANGLE() );

		while( !lodStack.empty() )
		{
			BARYCENTRIC_TRIANGLE tri = lodStack.top();
			lodStack.pop();

			if( tri.lod < LOD)
			{
				BARYCENTRIC_TRIANGLE tri_a, tri_b, tri_c, tri_d;

				PatchTesselate( tri, tri_a, tri_b, tri_c, tri_d );
				lodStack.push( tri_a );
				lodStack.push( tri_b );
				lodStack.push( tri_c );
				lodStack.push( tri_d );
			} 
			else
			{
				if( tri.lod == LOD )
				{
					unsigned int uii, uij;
					// ����Ʈ �ε����� ������ ���� �ʵ��� ū �޽ø� �����ؾ� �Ѵٴ� ���� ����ϼ���(����)
					BYTE i0 = (BYTE) m_indexData[ (index*3) + 0 ];
					BYTE i1 = (BYTE) m_indexData[ (index*3) + 1 ];
					BYTE i2 = (BYTE) m_indexData[ (index*3) + 2 ];

					if( gBarycentricPrecision == FLOAT_BARYCENTRIC )
					{
						*((float*)outStream+0) = tri.a.getI();
						*((float*)outStream+1) = tri.a.getJ();
						*(outStream+2) = (i0<<16) | (i1 << 8) | (i2 << 0);
						*((float*)outStream+3) = tri.b.getI();
						*((float*)outStream+4) = tri.b.getJ();
						*(outStream+5) = (i0<<16) | (i1 << 8) | (i2 << 0);
						*((float*)outStream+6) = tri.c.getI();
						*((float*)outStream+7) = tri.c.getJ();
						*(outStream+8) = (i0<<16) | (i1 << 8) | (i2 << 0);

						outStream += 9;
					} else
					{
						// ��� ��ǥ
						if( gBarycentricPrecision == BYTE_BARYCENTRIC )
						{
							uii = tri.a.getByteQuantisedI();
							uij = tri.a.getByteQuantisedJ();
							*(outStream+0) = (uii<<16) | (uij<<8); // D3DCOLOR ���� Ǯ�⿡ ����
							uii = tri.b.getByteQuantisedI();
							uij = tri.b.getByteQuantisedJ();
							*(outStream+2) = (uii<<16) | (uij<<8); // D3DCOLOR ���� Ǯ�⿡ ����
							uii = tri.c.getByteQuantisedI();
							uij = tri.c.getByteQuantisedJ();
							*(outStream+4) = (uii<<16) | (uij<<8); /// D3DCOLOR ���� Ǯ�⿡ ����
						} else if( gBarycentricPrecision == WORD_BARYCENTRIC )
						{
							uii = tri.a.getWordQuantisedI();
							uij = tri.a.getWordQuantisedJ();
							*(outStream+0) = (uii<<0) | (uij<<16); 
							uii = tri.b.getWordQuantisedI();
							uij = tri.b.getWordQuantisedJ();
							*(outStream+2) = (uii<<0) | (uij<<16); 
							uii = tri.c.getWordQuantisedI();
							uij = tri.c.getWordQuantisedJ();
							*(outStream+4) = (uii<<0) | (uij<<16); 
						}

						*(outStream+1) = (i0<<16) | (i1 << 8) | (i2 << 0);
						*(outStream+3) = (i0<<16) | (i1 << 8) | (i2 << 0);
						*(outStream+5) = (i0<<16) | (i1 << 8) | (i2 << 0);
						outStream += 6;
					}
				}
			}
		}
	}

	// ���� ���� ������ ����� �����մϴ�.
	hRes = m_pVB->Unlock();
	if( FAILED(hRes) )
		return hRes;

	return S_OK;
}


// InvalidateDeviceObjects()�� ���� ��ġ�� ���������� ��� Reset()�Ǿ��� �� �ֽ��ϴ�.  
// ���ҽ� D3DPOOL_DEFAULT �� ���� �߿� ���ӵǴ� ��Ÿ ��ġ ���� �������� ���⿡�� �����Ǿ�� �մϴ�.  
// ������ ����, ���, �ؽ�ó, �� ������ �߿� ������� �ʴ� ������ ���⿡�� �� ���� �����ϸ� �˴ϴ�.
// Render() �Ǵ� FrameMove() �� �ߺ��� ���� ������ �����մϴ�.
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	// TODO: ���� ���� ����
	
	// ��Ƽ���� ����
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

	// �ؽ�ó ����
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	// ��Ÿ ������ ���� ����
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x000F0F0F );

	// ���� ��Ʈ���� ����
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );
	m_matWorld = matIdentity;


	// �� ��Ʈ������ �����մϴ�. �� ��Ʈ������ ������ �������� ������ �� �ֽ��ϴ�.
	// �� ������ ���� �����Դϴ�. 
	// ���⼭�� z���� ���� �ڷ� 5����, �������� 3������ ���캾�ϴ�.
	// ������ �����ϰ� "����"�� y ������ �ǵ��� �����մϴ�.
    D3DXMATRIX matView;
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -3.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	m_matView = matView;

	// ���� ��� ����
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	m_matProj = matProj;

	// ���� ���� ����
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	// �۲� ����
    m_pFont->RestoreDeviceObjects();

    return S_OK;
}

//�����Ӵ� �� ���� ȣ��Ǹ� �� ȣ���� �ִϸ��̼��� �������Դϴ�.
//���.
HRESULT CMyD3DApplication::FrameMove()
{
	// ����� �Է� ���� ������Ʈ
    UpdateInput( &m_UserInput );

	// ����� �Է¿� ���� ���� ���¸� ������Ʈ�մϴ�.
    D3DXMATRIX matWorld;
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;

    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;

    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );

    D3DXMatrixMultiply( &matWorld, &matRotX, &matRotY );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_matWorld = matWorld;


	// LOD ����(��� ������ ���)
	if( m_UserInput.b1 )
	{
		if( m_displayMethod == DM_NPATCH )
			GenerateNPatchBuffers( 0 );
		else if( m_displayMethod == DM_CONTROL_NPATCH )
			GenerateD3DNPatchMesh( 1 );
		else if( m_displayMethod == DM_LINEAR )
			GenerateLinearBuffers( 0 );
	}
	else if( m_UserInput.b2 )
	{
		if( m_displayMethod == DM_NPATCH )
			GenerateNPatchBuffers( 1 );
		else if( m_displayMethod == DM_CONTROL_NPATCH )
			GenerateD3DNPatchMesh( 2 );
		else if( m_displayMethod == DM_LINEAR )
			GenerateLinearBuffers( 1 );
	} 
	else if( m_UserInput.b3 )
	{
		if( m_displayMethod == DM_NPATCH )
			GenerateNPatchBuffers( 2 );
		else if( m_displayMethod == DM_CONTROL_NPATCH )
			GenerateD3DNPatchMesh( 3 );
		else if( m_displayMethod == DM_LINEAR )
			GenerateLinearBuffers( 2 );
	} 
	else if( m_UserInput.b4 )
	{
		if( m_displayMethod == DM_NPATCH )
			GenerateNPatchBuffers( 3 );
		else if( m_displayMethod == DM_CONTROL_NPATCH )
			GenerateD3DNPatchMesh( 4 );
		else if( m_displayMethod == DM_LINEAR )
			GenerateLinearBuffers( 3 );
		
	} 
	else if( m_UserInput.b5 )
	{
		if( m_displayMethod == DM_NPATCH )
			GenerateNPatchBuffers( 4 );
		else if( m_displayMethod == DM_CONTROL_NPATCH )
			GenerateD3DNPatchMesh( 5 );
		else if( m_displayMethod == DM_LINEAR )
			GenerateLinearBuffers( 4 );
		
	}
	else if( m_UserInput.b6 )
	{
		if( m_displayMethod == DM_NPATCH )
			GenerateNPatchBuffers( 5 );
		else if( m_displayMethod == DM_CONTROL_NPATCH )
			GenerateD3DNPatchMesh( 6 );
		else if( m_displayMethod == DM_LINEAR )
			GenerateLinearBuffers( 5 );
	}
	
	// �޼ҵ� ����
	if( m_UserInput.bf5 && m_displayMethod != DM_CONTROL)
		m_displayMethod = DM_CONTROL;
	else if( m_UserInput.bf6 && m_displayMethod != DM_CONTROL_NPATCH)
	{
		m_displayMethod = DM_CONTROL_NPATCH;
		GenerateD3DNPatchMesh( m_lod - 1  );
	} 
	else if( m_UserInput.bf7 && m_displayMethod != DM_BARYCENTRIC)
		m_displayMethod = DM_BARYCENTRIC;

	else if( m_UserInput.bf8 && m_displayMethod != DM_NPATCH)
	{
		m_displayMethod = DM_NPATCH;
		GenerateNPatchBuffers( m_lod );
	} 
	else if( m_UserInput.bf9 && m_displayMethod != DM_LINEAR)
	{
		m_displayMethod = DM_LINEAR;
		GenerateLinearBuffers( m_lod );
	}
	return SetupDisplayMethod();
}

//����� �Է��� ������Ʈ�մϴ�.  �����Ӵ� �� ���� ȣ��˴ϴ�.
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    HRESULT hr;

	// �Է��� ��ġ ���¸� �������� ���¸� �帮�� ǥ���մϴ�.
    ZeroMemory( &pUserInput->diks, sizeof(pUserInput->diks) );
    hr = m_pKeyboard->GetDeviceState( sizeof(pUserInput->diks), &pUserInput->diks );
    if( FAILED(hr) ) 
    {
        m_pKeyboard->Acquire();
        return; 
    }

    pUserInput->bRotateLeft  = ( (pUserInput->diks[DIK_LEFT] & 0x80)  == 0x80 );
    pUserInput->bRotateRight = ( (pUserInput->diks[DIK_RIGHT] & 0x80) == 0x80 );
    pUserInput->bRotateUp    = ( (pUserInput->diks[DIK_UP] & 0x80)    == 0x80 );
    pUserInput->bRotateDown  = ( (pUserInput->diks[DIK_DOWN] & 0x80)  == 0x80 );

	pUserInput->b1 = ( (pUserInput->diks[DIK_1] & 0x80)  == 0x80 );
	pUserInput->b2 = ( (pUserInput->diks[DIK_2] & 0x80)  == 0x80 );
	pUserInput->b3 = ( (pUserInput->diks[DIK_3] & 0x80)  == 0x80 );
	pUserInput->b4 = ( (pUserInput->diks[DIK_4] & 0x80)  == 0x80 );
	pUserInput->b5 = ( (pUserInput->diks[DIK_5] & 0x80)  == 0x80 );
	pUserInput->b6 = ( (pUserInput->diks[DIK_6] & 0x80)  == 0x80 );
	pUserInput->b7 = ( (pUserInput->diks[DIK_7] & 0x80)  == 0x80 );
	pUserInput->b8 = ( (pUserInput->diks[DIK_8] & 0x80)  == 0x80 );
	pUserInput->b9 = ( (pUserInput->diks[DIK_9] & 0x80)  == 0x80 );

	pUserInput->bf5 = ( (pUserInput->diks[DIK_F5] & 0x80)  == 0x80 );
	pUserInput->bf6 = ( (pUserInput->diks[DIK_F6] & 0x80)  == 0x80 );
	pUserInput->bf7 = ( (pUserInput->diks[DIK_F7] & 0x80)  == 0x80 );

	pUserInput->bf8 = ( (pUserInput->diks[DIK_F8] & 0x80)  == 0x80 );
	pUserInput->bf9 = ( (pUserInput->diks[DIK_F9] & 0x80)  == 0x80 );
}

// �����Ӵ� �� �� ȣ��Ǹ� �� ȣ���� 3D�� �������Դϴ�.
// ������. 
// �� ����� ������ ���¸� �����ϰ� ����Ʈ�� �����ϰ� ����� �������մϴ�.
HRESULT CMyD3DApplication::Render()
{
	// ����Ʈ �����
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x50505050, 1.0f, 0L );
	//m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	
    // ��� ����
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		// MVP ���
		D3DXMATRIX matMVP;
        D3DXMatrixMultiply( &matMVP, &m_matView, &m_matProj );
        D3DXMatrixMultiply( &matMVP, &m_matWorld, &matMVP );

		// ���� ȿ�� Ǯ�� ����ؾ� ������ �̴� ���� ������ ���Դϴ�...
		m_controlFX->SetMatrix( "MVP", &matMVP );
		m_baryFX->SetMatrix( "MVP", &matMVP );
		m_npatchFX->SetMatrix( "MVP", &matMVP );
		m_linearFX->SetMatrix( "MVP", &matMVP );

		unsigned int numPasses;

		switch( m_displayMethod )
		{
		case DM_CONTROL:
			m_controlFX->Begin( &numPasses, 0 );
			for(unsigned int i=0; i < numPasses;i++)
			{
				m_controlFX->BeginPass( i );
				m_pD3DXMesh->DrawSubset( 0 );
			}
			m_controlFX->End();
			break;
		case DM_CONTROL_NPATCH:
			m_controlFX->Begin( &numPasses, 0 );
			for(unsigned int i=0; i < numPasses;i++)
			{
				m_controlFX->BeginPass( i );
				m_pD3DXPatchMeshDest->DrawSubset( 0 );
			}
			m_controlFX->End();
			break;
		case DM_BARYCENTRIC:
			m_baryFX->Begin( &numPasses, 0 );
			for(unsigned int i=0; i < numPasses;i++)
			{
				m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(unsigned int) );
				m_pd3dDevice->SetVertexDeclaration( m_pBaryDecl );
				m_baryFX->BeginPass( i );
				m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, m_numIndices /3 );
			}
			m_baryFX->End();
			break;
		case DM_NPATCH:
			m_npatchFX->Begin( &numPasses, 0 );
			for(unsigned int i=0; i < numPasses;i++)
			{
				m_pd3dDevice->SetVertexDeclaration( m_pNPatchDecl );
				m_npatchFX->BeginPass( i );
				const unsigned int numTri = m_numIndices / 3;
				unsigned int numVerts = calcPatchVertsPerOrigTri( m_lod ) * numTri;

				unsigned int vSize;
				if( bFloatNPatch == false )
					vSize = sizeof(unsigned int);
				else
					vSize = sizeof(float) * 3;

				// �Ϻ� ī��� �� ���� ���� ������ ó������ ���մϴ�.
				if( numVerts > 65536 )
					numVerts = 65536;

				m_pd3dDevice->SetStreamSource(	0, m_pVB, 0, vSize );
				m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, numVerts /3 );
			}
			m_npatchFX->End();
			break;
		case DM_LINEAR:
			m_linearFX->Begin( &numPasses, 0 );
			for(unsigned int i=0; i < numPasses;i++)
			{
				m_pd3dDevice->SetVertexDeclaration( m_pLinearDecl );
				m_linearFX->BeginPass( i );
				const unsigned int numTri = m_numIndices / 3;
				unsigned int numVerts = calcPatchVertsPerOrigTri( m_lod ) * numTri;

				unsigned int vSize;
				if( gBarycentricPrecision == FLOAT_BARYCENTRIC )
					vSize = sizeof(float)*2 + sizeof(unsigned int);
				else
					vSize = sizeof(unsigned int) * 2;

				// �Ϻ� ī��� �� ���� ���� ������ ó������ ���մϴ�.
				if( numVerts > 65536 )
					numVerts = 65536;

				m_pd3dDevice->SetStreamSource(	0, m_pVB, 0, vSize );
				m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, numVerts /3 );
			}
			m_linearFX->End();
			break;
		}

		// ������ ��� �� ���� �ؽ�Ʈ
        RenderText();

		// ����� �����մϴ�.
        m_pd3dDevice->EndScene();
    }
    return S_OK;
}

// ���� ���� �ؽ�Ʈ�� ��鿡 �������մϴ�.
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

	// ���÷��� ��� ���
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

	fNextLine = 40.0f;
	// ��� �� ���� ���
	if( m_displayMethod == DM_BARYCENTRIC )
	{
		m_pFont->DrawText( 2, fNextLine, fontColor, "Vertex shader barycentric vertices" );
		fNextLine += 20.0f;
	} 
	else if( m_displayMethod == DM_NPATCH )
	{
		m_pFont->DrawText( 2, fNextLine, fontColor, "Vertex shader NPatch basis" );
		fNextLine += 20.0f;
	    sprintf( szMsg, "load level %d", m_lod );
		m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
	} 
	else if( m_displayMethod == DM_LINEAR )
	{
		m_pFont->DrawText( 2, fNextLine, fontColor, "Vertex shader linear basis" );
		fNextLine += 20.0f;
	    sprintf( szMsg, "load level %d", m_lod );
		m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
	} 
	else if( m_displayMethod == DM_CONTROL )
	{
		m_pFont->DrawText( 2, fNextLine, fontColor, "Standard D3DX mesh rendering" );
		fNextLine += 20.0f;
	    sprintf( szMsg, "load level %d", m_lod );
		m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
	} 
	else if( m_displayMethod == DM_CONTROL_NPATCH )
	{
		m_pFont->DrawText( 2, fNextLine, fontColor, "D3DX Software NPatch tesselation" );
		fNextLine += 20.0f;
	}

	fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    wsprintf( szMsg, TEXT("Arrow keys: Up=%d Down=%d Left=%d Right=%d"), m_UserInput.bRotateUp, m_UserInput.bRotateDown, m_UserInput.bRotateLeft, m_UserInput.bRotateRight );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    return S_OK;
}

// ���ÿ��� ����� ���� �޽����� ������ �� �ֵ��� �⺻ WndProc�� �������մϴ�.
// ó��(��: ���콺, Ű���� �Ǵ� �޴� ��� ó��).
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_PAINT:
        {
            if( m_bLoadingApp )
            {
				// â�� �׸��� �׷� ����ڿ��� ���� �ε� ������ �˸��ϴ�.
				// TODO: �ʿ信 ���� ����
                HDC hDC = GetDC( hWnd );
                TCHAR strMsg[MAX_PATH];
                wsprintf( strMsg, TEXT("Loading... Please wait") );
                RECT rct;
                GetClientRect( hWnd, &rct );
                DrawText( hDC, strMsg, -1, &rct, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
                ReleaseDC( hWnd, hDC );
            }
            break;
        }

    }

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}

//��ġ ��ü�� ��ȿȭ�մϴ�. 
//RestoreDeviceObjects()�� ����
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	// TODO: RestoreDeviceObjects()���� ������ ��� ��ü�� �����մϴ�.
    m_pFont->InvalidateDeviceObjects();
    return S_OK;
}

// InitDeviceObjects()�� ����
// ���� ����ǰų� ��Ⱑ ����� �� ȣ��˴ϴ�.
// �� �Լ��� ��� ��ġ ���� ��ü�� �����մϴ�.
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	CleanDisplayMethod();

	delete m_posVertexData; m_posVertexData = 0;
	delete m_normVertexData; m_normVertexData = 0;
	delete m_indexData; m_indexData = 0;
	m_numVertices = 0;
	m_numIndices = 0;

    m_pFont->DeleteDeviceObjects();

	SAFE_RELEASE( m_linearFX ) ;
	SAFE_RELEASE( m_controlFX ) ;
	SAFE_RELEASE( m_npatchFX ) ;
	SAFE_RELEASE( m_baryFX ) ;

	SAFE_RELEASE( m_pD3DXPatchMeshDest ); 
	SAFE_RELEASE( m_pD3DXPatchMesh );
	SAFE_RELEASE( m_pD3DXMesh );
    return S_OK;
}


void CMyD3DApplication::CleanDisplayMethod()
{
	SAFE_RELEASE( m_pVB );

	SAFE_RELEASE( m_pBaryDecl );
	SAFE_RELEASE( m_pNPatchDecl );
	SAFE_RELEASE( m_pLinearDecl );

	if (m_baryPos != nullptr)
	{
		delete m_baryPos; 
		m_baryPos = nullptr;
	}
	if (m_npatchPos != nullptr)
	{
		delete m_npatchPos; 
		m_npatchPos = nullptr;
	}
	if (m_npatchNorm != nullptr)
	{
		delete m_npatchNorm; 
		m_npatchNorm = nullptr;
	}
	if (m_linearPos != nullptr)
	{
		delete m_linearPos; 
		m_linearPos = nullptr;
	}
	if (m_linearNorm != nullptr)
	{
		delete m_linearNorm; 
		m_linearNorm = nullptr;
	}

}

HRESULT CMyD3DApplication::SetupDisplayMethod()
{
    HRESULT hr;

	if( m_displayMethod == DM_BARYCENTRIC )
	{
		if( m_pBaryDecl == 0 )
		{
			CleanDisplayMethod();
			if( FAILED( hr = GenerateBaryConstants() ) )
				return DXTRACE_ERR( "GenerateBaryConstants", hr );
		}
	} 
	else if( m_displayMethod == DM_NPATCH )
	{
		if( m_pNPatchDecl == 0 )
		{
			CleanDisplayMethod();
			if( FAILED( hr = GenerateNPatchConstants() ) )
				return DXTRACE_ERR( "GenerateNPatchConstants", hr );
		}
	} 
	else if( m_displayMethod == DM_LINEAR )
	{
		if( m_pLinearDecl == 0 )
		{
			CleanDisplayMethod();
			if( FAILED( hr = GenerateLinearConstants() ) )
				return DXTRACE_ERR( "GenerateLinearConstants", hr );
		}
	}
	return S_OK;
}

// OneTimeSceneInit()�� ����
// ���� ����Ǳ� ���� ȣ��˴ϴ�. 
// �� �Լ��� �ۿ� ��ȸ�� �����մϴ�.
// �� ��ü�� �����մϴ�.
HRESULT CMyD3DApplication::FinalCleanup()
{
	// TODO: �ʿ��� ���� ������ �����մϴ�.
	// D3D �۲� ����
    SAFE_DELETE( m_pFont );

	// DirectInput ����
    CleanupDirectInput();

	// ������Ʈ���� ������ ���ϴ�.
    WriteSettings();

    return S_OK;
}

//DirectInput ����
VOID CMyD3DApplication::CleanupDirectInput()
{
	// DirectX �Է� ��ü ����
    SAFE_RELEASE( m_pKeyboard );
    SAFE_RELEASE( m_pDI );

}