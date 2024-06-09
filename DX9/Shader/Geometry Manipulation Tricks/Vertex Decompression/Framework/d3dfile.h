#ifndef D3DFILE_H
#define D3DFILE_H
#include <tchar.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxfile.h>

class CD3DMesh
{
public:
    TCHAR               m_strName[512];

    LPD3DXMESH          m_pSysMemMesh;    // SysMem �޽�, ũ�� ������ ���� ����
    LPD3DXMESH          m_pLocalMesh;    // ���� �޽�, ũ�� ���� �� �ٽ� �ۼ���
    
    DWORD               m_dwNumMaterials;    // �޽� ���
    D3DMATERIAL9*       m_pMaterials;
    LPDIRECT3DTEXTURE9* m_pTextures;
    bool                m_bUseMaterials;

public:
    // ������
    HRESULT Render( LPDIRECT3DDEVICE9 pd3dDevice,bool bDrawOpaqueSubsets = true, bool bDrawAlphaSubsets = true );

    // �޽� ����
    LPD3DXMESH GetSysMemMesh() { return m_pSysMemMesh; }
    LPD3DXMESH GetLocalMesh()  { return m_pLocalMesh; }

    // ������ �ɼ�
    void    UseMeshMaterials( bool bFlag ) { m_bUseMaterials = bFlag; }
    HRESULT SetFVF( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwFVF );

    // �ʱ�ȭ ��
    HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT InvalidateDeviceObjects();

    // ����/�ı�
    HRESULT Create( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* strFilename );
    HRESULT Create( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECTXFILEDATA pFileData );
    HRESULT Destroy();

    CD3DMesh( TCHAR* strName = _T("CD3DFile_Mesh") );
    virtual ~CD3DMesh();
};

//���� ��� �޽ø� �ε��ϰ� �������ϱ� ���� Ŭ����
class CD3DFrame
{
public:
    TCHAR      m_strName[512];
    D3DXMATRIX m_mat;
    CD3DMesh*  m_pMesh;

    CD3DFrame* m_pNext;
    CD3DFrame* m_pChild;

public:
    // Matrix access
    void        SetMatrix( D3DXMATRIX* pmat ) { m_mat = *pmat; }
    D3DXMATRIX* GetMatrix()                   { return &m_mat; }

    CD3DMesh*   FindMesh( TCHAR* strMeshName );
    CD3DFrame*  FindFrame( TCHAR* strFrameName );
    bool        EnumMeshes( bool (*EnumMeshCB)(CD3DMesh*,void*),void* pContext );

    HRESULT Destroy();
    HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT InvalidateDeviceObjects();
    HRESULT Render( LPDIRECT3DDEVICE9 pd3dDevice, bool bDrawOpaqueSubsets = true, bool bDrawAlphaSubsets = true, D3DXMATRIX* pmatWorldMartix = NULL);
    
    CD3DFrame( TCHAR* strName = _T("CD3DFile_Frame") );
    virtual ~CD3DFrame();
};

class CD3DFile : public CD3DFrame
{
    HRESULT LoadMesh( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECTXFILEDATA pFileData, 
                      CD3DFrame* pParentFrame );
    HRESULT LoadFrame( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECTXFILEDATA pFileData, 
                       CD3DFrame* pParentFrame );
public:
    HRESULT Create( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* strFilename );
    HRESULT CreateFromResource( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* strResource, TCHAR* strType );

    // ���� ��ġ�� ��� ���� ��ȯ�� �����մϴ�. ���� Ʈ�������� �ƴ� ���
    // ���� ��ġ�� �����ϸ� �� �Լ��� �����մϴ�.
    HRESULT Render( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* pmatWorldMatrix = NULL );

    CD3DFile() : CD3DFrame( _T("CD3DFile_Root") ) {}
};
#endif