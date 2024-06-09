//DirectX .X ������ �ε��ϱ� ���� ���� �ڵ��Դϴ�.
#define STRICT
#include <tchar.h>
#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dxfile.h>
#include <rmxfguid.h>
#include <rmxftmpl.h>
#include "D3DFile.h"
#include "DXUtil.h"

CD3DMesh::CD3DMesh( TCHAR* strName )
{
    _tcsncpy( m_strName, strName, sizeof(m_strName) / sizeof(TCHAR) );
    m_strName[sizeof(m_strName) / sizeof(TCHAR) - 1] = _T('\0');
    m_pSysMemMesh        = NULL;
    m_pLocalMesh         = NULL;
    m_dwNumMaterials     = 0L;
    m_pMaterials         = NULL;
    m_pTextures          = NULL;
    m_bUseMaterials      = TRUE;
}

CD3DMesh::~CD3DMesh()
{
    Destroy();
}

HRESULT CD3DMesh::Create(LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* strFilename)
{
    TCHAR        strPath[MAX_PATH];
    LPD3DXBUFFER pAdjacencyBuffer = NULL;
    LPD3DXBUFFER pMtrlBuffer = NULL;
    HRESULT      hr;

    // ���� ��θ� ã�� ANSI�� ��ȯ�մϴ�(D3DX API��).
    DXUtil_FindMediaFileCb(strPath, sizeof(strPath), strFilename);

    // �޽� �ε�
    hr = D3DXLoadMeshFromX(strPath, D3DXMESH_SYSTEMMEM, pd3dDevice, &pAdjacencyBuffer, &pMtrlBuffer, NULL, &m_dwNumMaterials, &m_pSysMemMesh);
    if (FAILED(hr))
    {
        SAFE_RELEASE(pAdjacencyBuffer);
        SAFE_RELEASE(pMtrlBuffer);
        return hr;
    }

    // ������ ���� �޽ø� ����ȭ�մϴ�.
    hr = m_pSysMemMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL);
    if (FAILED(hr))
    {
        SAFE_RELEASE(pAdjacencyBuffer);
        SAFE_RELEASE(pMtrlBuffer);
        return hr;
    }

    // �޽��� ���� ��Ƽ���� ������ ����ϴ�.
    // ���ۿ��� ��� �迭�� �����ɴϴ�.
    if (pMtrlBuffer && m_dwNumMaterials > 0)
    {
        // ��Ƽ����� �ؽ�ó�� �޸𸮸� �Ҵ��մϴ�.
        D3DXMATERIAL* d3dxMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
        m_pMaterials = new D3DMATERIAL9[m_dwNumMaterials];
        if (!m_pMaterials)
        {
            hr = E_OUTOFMEMORY;
            SAFE_RELEASE(pAdjacencyBuffer);
            SAFE_RELEASE(pMtrlBuffer);
            return hr;
        }

        m_pTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];
        if (!m_pTextures)
        {
            hr = E_OUTOFMEMORY;
            SAFE_RELEASE(pAdjacencyBuffer);
            SAFE_RELEASE(pMtrlBuffer);
            delete[] m_pMaterials;
            m_pMaterials = NULL;
            return hr;
        }

        // �� ��Ḧ �����ϰ� �ؽ�ó�� ����ϴ�.
        for (DWORD i = 0; i < m_dwNumMaterials; i++)
        {
            // �ڷ� ����
            m_pMaterials[i] = d3dxMtrls[i].MatD3D;
            m_pTextures[i] = NULL;

            // �ؽ�ó ����
            if (d3dxMtrls[i].pTextureFilename)
            {
                TCHAR strTexture[MAX_PATH];
                TCHAR strTextureTemp[MAX_PATH];
                DXUtil_ConvertAnsiStringToGenericCb(strTextureTemp, d3dxMtrls[i].pTextureFilename, sizeof(strTextureTemp));
                DXUtil_FindMediaFileCb(strTexture, sizeof(strTexture), strTextureTemp);

                if (FAILED(D3DXCreateTextureFromFile(pd3dDevice, strTexture, &m_pTextures[i])))
                {
                    m_pTextures[i] = NULL;
                }
            }
        }
    }

    SAFE_RELEASE(pAdjacencyBuffer);
    SAFE_RELEASE(pMtrlBuffer);

    return S_OK;
}

HRESULT CD3DMesh::Create(LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECTXFILEDATA pFileData)
{
    LPD3DXBUFFER pMtrlBuffer = NULL;
    LPD3DXBUFFER pAdjacencyBuffer = NULL;
    HRESULT hr = E_FAIL; // �ʱ�ȭ

    // DXFILEDATA ��ü�κ��� �޽� �ε�
    hr = D3DXLoadMeshFromXof((LPD3DXFILEDATA)pFileData, D3DXMESH_SYSTEMMEM, pd3dDevice, &pAdjacencyBuffer, &pMtrlBuffer, NULL, &m_dwNumMaterials, &m_pSysMemMesh);
    if (FAILED(hr))
    {
        // ���� �� �޸� ���� �� ����
        SAFE_RELEASE(pAdjacencyBuffer);
        SAFE_RELEASE(pMtrlBuffer);
        return hr;
    }

    // ���� ����ȭ�� ���� �޽� ����ȭ
    hr = m_pSysMemMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, (DWORD*)pAdjacencyBuffer->GetBufferPointer(), NULL, NULL, NULL);
    if (FAILED(hr))
    {
        // ���� �� �޸� ���� �� ����
        SAFE_RELEASE(pAdjacencyBuffer);
        SAFE_RELEASE(pMtrlBuffer);
        return hr;
    }

    // �޽��� ��Ƽ���� ������ ����ϴ�.
    // ���ۿ��� ��Ƽ���� �迭�� �����ɴϴ�.
    if (pMtrlBuffer && m_dwNumMaterials > 0)
    {
        // ��Ƽ���� �� �ؽ�ó�� �޸𸮸� �Ҵ��մϴ�.
        D3DXMATERIAL* d3dxMtrls = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
        m_pMaterials = new D3DMATERIAL9[m_dwNumMaterials];
        m_pTextures = new LPDIRECT3DTEXTURE9[m_dwNumMaterials];
        if (!m_pMaterials || !m_pTextures)
        {
            // �Ҵ� ���� �� �޸� ���� �� ����
            hr = E_OUTOFMEMORY;
            SAFE_RELEASE(pAdjacencyBuffer);
            SAFE_RELEASE(pMtrlBuffer);
            return hr;
        }

        // �� ��Ƽ������ �����ϰ� �ش� �ؽ�ó�� ����ϴ�.
        for (DWORD i = 0; i < m_dwNumMaterials; i++)
        {
            // ��Ƽ���� ����
            m_pMaterials[i] = d3dxMtrls[i].MatD3D;
            m_pTextures[i] = NULL;

            // �ؽ�ó ����
            if (d3dxMtrls[i].pTextureFilename)
            {
                TCHAR strTexture[MAX_PATH];
                TCHAR strTextureTemp[MAX_PATH];
                DXUtil_ConvertAnsiStringToGenericCb(strTextureTemp, d3dxMtrls[i].pTextureFilename, sizeof(strTextureTemp));
                DXUtil_FindMediaFileCb(strTexture, sizeof(strTexture), strTextureTemp);

                if (FAILED(D3DXCreateTextureFromFile(pd3dDevice, strTexture, &m_pTextures[i])))
                {
                    // �ؽ�ó ���� ���� �� NULL�� ����
                    m_pTextures[i] = NULL;
                }
            }
        }
    }

    // �������� ��� ���� �ڵ� ��ȯ
    hr = S_OK;

    // �޸� ���� �� ����
    SAFE_RELEASE(pAdjacencyBuffer);
    SAFE_RELEASE(pMtrlBuffer);

    return hr;
}

HRESULT CD3DMesh::SetFVF( LPDIRECT3DDEVICE9 pd3dDevice, DWORD dwFVF )
{
    LPD3DXMESH pTempSysMemMesh = NULL;
    LPD3DXMESH pTempLocalMesh  = NULL;

    if( m_pSysMemMesh )
    {
        if( FAILED( m_pSysMemMesh->CloneMeshFVF( D3DXMESH_SYSTEMMEM, dwFVF, pd3dDevice, &pTempSysMemMesh ) ) )
            return E_FAIL;
    }
    if( m_pLocalMesh )
    {
        if( FAILED( m_pLocalMesh->CloneMeshFVF( 0L, dwFVF, pd3dDevice, &pTempLocalMesh ) ) )
        {
            SAFE_RELEASE( pTempSysMemMesh );
            return E_FAIL;
        }
    }

    SAFE_RELEASE( m_pSysMemMesh );
    SAFE_RELEASE( m_pLocalMesh );

    if( pTempSysMemMesh ) m_pSysMemMesh = pTempSysMemMesh;
    if( pTempLocalMesh )  m_pLocalMesh  = pTempLocalMesh;

    // �޽ÿ� ������ �ִ� ��� ������ ����մϴ�.
    if( m_pSysMemMesh )
        D3DXComputeNormals( m_pSysMemMesh, NULL );
    if( m_pLocalMesh )
        D3DXComputeNormals( m_pLocalMesh, NULL );

    return S_OK;
}

HRESULT CD3DMesh::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
    if( NULL == m_pSysMemMesh )
        return E_FAIL;


    // �޽��� ���� �޸� ������ ����ϴ�. ����: �츮�� �������� �ֱ� ������
    // �÷��װ� �����ϴ�. �⺻ ������ ���� �޸𸮿� �����ϴ� ���Դϴ�.
    if( FAILED( m_pSysMemMesh->CloneMeshFVF( 0L, m_pSysMemMesh->GetFVF(),pd3dDevice, &m_pLocalMesh ) ) )
        return E_FAIL;

    return S_OK;
}

HRESULT CD3DMesh::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pLocalMesh );

    return S_OK;
}

HRESULT CD3DMesh::Destroy()
{
    InvalidateDeviceObjects();
    for( UINT i=0; i<m_dwNumMaterials; i++ )
        SAFE_RELEASE( m_pTextures[i] );
    SAFE_DELETE_ARRAY( m_pTextures );
    SAFE_DELETE_ARRAY( m_pMaterials );

    SAFE_RELEASE( m_pSysMemMesh );

    m_dwNumMaterials = 0L;

    return S_OK;
}

HRESULT CD3DMesh::Render( LPDIRECT3DDEVICE9 pd3dDevice, bool bDrawOpaqueSubsets, bool bDrawAlphaSubsets )
{
    if( NULL == m_pLocalMesh )
        return E_FAIL;

    // ����, ���� ���� �κ� ������ �׸��ϴ�.
    if( bDrawOpaqueSubsets )
    {
        for( DWORD i=0; i<m_dwNumMaterials; i++ )
        {
            if( m_bUseMaterials )
            {
                if( m_pMaterials[i].Diffuse.a < 1.0f )
                    continue;
                pd3dDevice->SetMaterial( &m_pMaterials[i] );
                pd3dDevice->SetTexture( 0, m_pTextures[i] );
            }
            m_pLocalMesh->DrawSubset( i );
        }
    }

    // �׷� ���� ���ĸ� ����Ͽ� ���� ������ �׸��ϴ�.
    if( bDrawAlphaSubsets && m_bUseMaterials )
    {
        for( DWORD i=0; i<m_dwNumMaterials; i++ )
        {
            if( m_pMaterials[i].Diffuse.a == 1.0f )
                continue;

            // ������ ������ �����մϴ�.
            pd3dDevice->SetMaterial( &m_pMaterials[i] );
            pd3dDevice->SetTexture( 0, m_pTextures[i] );
            m_pLocalMesh->DrawSubset( i );
        }
    }

    return S_OK;
}

CD3DFrame::CD3DFrame( TCHAR* strName )
{
    _tcsncpy( m_strName, strName, sizeof(m_strName) / sizeof(TCHAR) );
    m_strName[sizeof(m_strName) / sizeof(TCHAR) - 1] = _T('\0');
    D3DXMatrixIdentity( &m_mat );
    m_pMesh  = NULL;

    m_pChild = NULL;
    m_pNext  = NULL;
}

CD3DFrame::~CD3DFrame()
{
    SAFE_DELETE( m_pChild );
    SAFE_DELETE( m_pNext );
}

bool CD3DFrame::EnumMeshes( bool (*EnumMeshCB)(CD3DMesh*,void*),
                            void* pContext )
{
    if( m_pMesh )
        EnumMeshCB( m_pMesh, pContext );
    if( m_pChild )
        m_pChild->EnumMeshes( EnumMeshCB, pContext );
    if( m_pNext )
        m_pNext->EnumMeshes( EnumMeshCB, pContext );

    return TRUE;
}

CD3DMesh* CD3DFrame::FindMesh( TCHAR* strMeshName )
{
    CD3DMesh* pMesh;

    if( m_pMesh )
        if( !lstrcmpi( m_pMesh->m_strName, strMeshName ) )
            return m_pMesh;

    if( m_pChild )
        if( NULL != ( pMesh = m_pChild->FindMesh( strMeshName ) ) )
            return pMesh;

    if( m_pNext )
        if( NULL != ( pMesh = m_pNext->FindMesh( strMeshName ) ) )
            return pMesh;

    return NULL;
}

CD3DFrame* CD3DFrame::FindFrame( TCHAR* strFrameName )
{
    CD3DFrame* pFrame;

    if( !lstrcmpi( m_strName, strFrameName ) )
        return this;

    if( m_pChild )
        if( NULL != ( pFrame = m_pChild->FindFrame( strFrameName ) ) )
            return pFrame;

    if( m_pNext )
        if( NULL != ( pFrame = m_pNext->FindFrame( strFrameName ) ) )
            return pFrame;

    return NULL;
}

HRESULT CD3DFrame::Destroy()
{
    if( m_pMesh )  m_pMesh->Destroy();
    if( m_pChild ) m_pChild->Destroy();
    if( m_pNext )  m_pNext->Destroy();

    SAFE_DELETE( m_pMesh );
    SAFE_DELETE( m_pNext );
    SAFE_DELETE( m_pChild );

    return S_OK;
}

HRESULT CD3DFrame::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
    if( m_pMesh )  m_pMesh->RestoreDeviceObjects( pd3dDevice );
    if( m_pChild ) m_pChild->RestoreDeviceObjects( pd3dDevice );
    if( m_pNext )  m_pNext->RestoreDeviceObjects( pd3dDevice );
    return S_OK;
}

HRESULT CD3DFrame::InvalidateDeviceObjects()
{
    if( m_pMesh )  m_pMesh->InvalidateDeviceObjects();
    if( m_pChild ) m_pChild->InvalidateDeviceObjects();
    if( m_pNext )  m_pNext->InvalidateDeviceObjects();
    return S_OK;
}

HRESULT CD3DFrame::Render( LPDIRECT3DDEVICE9 pd3dDevice, bool bDrawOpaqueSubsets, bool bDrawAlphaSubsets, D3DXMATRIX* pmatWorldMatrix )
{
    // ���� ��ġ�� ��� ���� ��ȯ�� �����մϴ�. ���� Ʈ�������� �ƴ� ���
    // ���� ��ġ�� �����ϸ� �� �Լ��� �����մϴ�.

    D3DXMATRIXA16 matSavedWorld, matWorld;

    if ( NULL == pmatWorldMatrix )
        pd3dDevice->GetTransform( D3DTS_WORLD, &matSavedWorld );
    else
        matSavedWorld = *pmatWorldMatrix;

    D3DXMatrixMultiply( &matWorld, &m_mat, &matSavedWorld );
    pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    if( m_pMesh )
        m_pMesh->Render( pd3dDevice, bDrawOpaqueSubsets, bDrawAlphaSubsets );

    if( m_pChild )
        m_pChild->Render( pd3dDevice, bDrawOpaqueSubsets, bDrawAlphaSubsets, &matWorld );

    pd3dDevice->SetTransform( D3DTS_WORLD, &matSavedWorld );

    if( m_pNext )
        m_pNext->Render( pd3dDevice, bDrawOpaqueSubsets, bDrawAlphaSubsets, &matSavedWorld );

    return S_OK;
}

HRESULT CD3DFile::LoadFrame( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECTXFILEDATA pFileData, CD3DFrame* pParentFrame )
{
    LPDIRECTXFILEDATA   pChildData = NULL;
    LPDIRECTXFILEOBJECT pChildObj = NULL;
    const GUID* pGUID;
    DWORD       cbSize;
    CD3DFrame*  pCurrentFrame;
    HRESULT     hr;

    //��ü�� ������ �����ɴϴ�.
    if( FAILED( hr = pFileData->GetType( &pGUID ) ) )
        return hr;

    if( *pGUID == TID_D3DRMMesh )
    {
        hr = LoadMesh( pd3dDevice, pFileData, pParentFrame );
        if( FAILED(hr) )
            return hr;
    }
    if( *pGUID == TID_D3DRMFrameTransformMatrix )
    {
        D3DXMATRIX* pmatMatrix;
        hr = pFileData->GetData( NULL, &cbSize, (void**)&pmatMatrix );
        if( FAILED(hr) )
            return hr;

        // �θ��� ����� �� ��ķ� ������Ʈ�մϴ�.
        pParentFrame->SetMatrix( pmatMatrix );
    }
    if( *pGUID == TID_D3DRMFrame )
    {
        // ������ �̸��� �����ɴϴ�.
        CHAR  strAnsiName[512] = "";
        TCHAR strName[512];
        DWORD dwNameLength = 512;
        if( FAILED( hr = pFileData->GetName( strAnsiName, &dwNameLength ) ) )
            return hr;
        DXUtil_ConvertAnsiStringToGenericCb( strName, strAnsiName, sizeof(strName) );

        // ������ ����
        pCurrentFrame = new CD3DFrame( strName );
        if( pCurrentFrame == NULL )
            return E_OUTOFMEMORY;

        pCurrentFrame->m_pNext = pParentFrame->m_pChild;
        pParentFrame->m_pChild = pCurrentFrame;


        // �ڽ� ��ü ����
        while( SUCCEEDED( pFileData->GetNextObject( &pChildObj ) ) )
        {
            // �ڽĿ��� FileData�� �����մϴ�.
            hr = pChildObj->QueryInterface( IID_IDirectXFileData,
                                            (void**)&pChildData );
            if( SUCCEEDED(hr) )
            {
                hr = LoadFrame( pd3dDevice, pChildData, pCurrentFrame );
                pChildData->Release();
            }

            pChildObj->Release();

            if( FAILED(hr) )
                return hr;
        }
    }

    return S_OK;
}

HRESULT CD3DFile::LoadMesh( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECTXFILEDATA pFileData, CD3DFrame* pParentFrame )
{
    // ����� �����Ӵ� �޽� �ϳ��� ���˴ϴ�.
    if( pParentFrame->m_pMesh )
        return E_FAIL;

    // �޽� �̸��� �����ɴϴ�
    CHAR  strAnsiName[512] = {0};
    TCHAR strName[512];
    DWORD dwNameLength = 512;
    HRESULT hr;
    if( FAILED( hr = pFileData->GetName( strAnsiName, &dwNameLength ) ) )
        return hr;
    DXUtil_ConvertAnsiStringToGenericCb( strName, strAnsiName, sizeof(strName) );

    // �޽� ����
    pParentFrame->m_pMesh = new CD3DMesh( strName );
    if( pParentFrame->m_pMesh == NULL )
        return E_OUTOFMEMORY;
    pParentFrame->m_pMesh->Create( pd3dDevice, pFileData );

    return S_OK;
}

HRESULT CD3DFile::CreateFromResource( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* strResource, TCHAR* strType )
{
    LPDIRECTXFILE           pDXFile   = NULL;
    LPDIRECTXFILEENUMOBJECT pEnumObj  = NULL;
    LPDIRECTXFILEDATA       pFileData = NULL;
    HRESULT hr;

    // x ���� ��ü ����
    if( FAILED( hr = DirectXFileCreate( &pDXFile ) ) )
        return E_FAIL;

    // d3drm �� ��ġ Ȯ���� ���� ���ø��� ����մϴ�.
    if( FAILED( hr = pDXFile->RegisterTemplates( (void*)D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES ) ) )
    {
        pDXFile->Release();
        return E_FAIL;
    }
    
    CHAR strTypeAnsi[MAX_PATH];
    DXUtil_ConvertGenericStringToAnsiCb( strTypeAnsi, strType, sizeof(strTypeAnsi) );

    DXFILELOADRESOURCE dxlr;
    dxlr.hModule = NULL;
    dxlr.lpName = strResource;
    dxlr.lpType = (TCHAR*) strTypeAnsi;

    // ������ ��ü ����
    hr = pDXFile->CreateEnumObject( (void*)&dxlr, DXFILELOAD_FROMRESOURCE, 
                                    &pEnumObj );
    if( FAILED(hr) )
    {
        pDXFile->Release();
        return hr;
    }

    // �ֻ��� ��ü(�׻� ��������)�� �����մϴ�.
    while( SUCCEEDED( pEnumObj->GetNextDataObject( &pFileData ) ) )
    {
        hr = LoadFrame( pd3dDevice, pFileData, this );
        pFileData->Release();
        if( FAILED(hr) )
        {
            pEnumObj->Release();
            pDXFile->Release();
            return E_FAIL;
        }
    }

    SAFE_RELEASE( pFileData );
    SAFE_RELEASE( pEnumObj );
    SAFE_RELEASE( pDXFile );

    return S_OK;
}

HRESULT CD3DFile::Create( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* strFilename )
{
    LPDIRECTXFILE           pDXFile   = NULL;
    LPDIRECTXFILEENUMOBJECT pEnumObj  = NULL;
    LPDIRECTXFILEDATA       pFileData = NULL;
    HRESULT hr;

    // x ���� ��ü ����
    if( FAILED( hr = DirectXFileCreate( &pDXFile ) ) )
        return E_FAIL;

    // d3drm �� ��ġ Ȯ���� ���� ���ø��� ����մϴ�.
    if( FAILED( hr = pDXFile->RegisterTemplates( (void*)D3DRM_XTEMPLATES,
                                                 D3DRM_XTEMPLATE_BYTES ) ) )
    {
        pDXFile->Release();
        return E_FAIL;
    }

    // ���� ��θ� ã�� ANSI�� ��ȯ�մϴ�(D3DXOF API��).
    TCHAR strPath[MAX_PATH];
    CHAR  strPathANSI[MAX_PATH];
    DXUtil_FindMediaFileCb( strPath, sizeof(strPath), strFilename );
    DXUtil_ConvertGenericStringToAnsiCb( strPathANSI, strPath, sizeof(strPathANSI) );
    
    // ������ ��ü ����
    hr = pDXFile->CreateEnumObject( (void*)strPathANSI, DXFILELOAD_FROMFILE, &pEnumObj );
    if( FAILED(hr) )
    {
        pDXFile->Release();
        return hr;
    }

    // �ֻ��� ��ü(�׻� ��������)�� �����մϴ�.
    while( SUCCEEDED( pEnumObj->GetNextDataObject( &pFileData ) ) )
    {
        hr = LoadFrame( pd3dDevice, pFileData, this );
        pFileData->Release();
        if( FAILED(hr) )
        {
            pEnumObj->Release();
            pDXFile->Release();
            return E_FAIL;
        }
    }

    SAFE_RELEASE( pFileData );
    SAFE_RELEASE( pEnumObj );
    SAFE_RELEASE( pDXFile );

    return S_OK;
}

HRESULT CD3DFile::Render( LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIX* pmatWorldMatrix )
{

    // ���� ��ġ�� ��� ���� ��ȯ�� �����մϴ�. ���� Ʈ�������� �ƴ� ���
    // ���� ��ġ�� �����ϸ� �� �Լ��� �����մϴ�.

    // ���� ��ȯ ����
    D3DXMATRIX matSavedWorld, matWorld;

    if ( NULL == pmatWorldMatrix )
        pd3dDevice->GetTransform( D3DTS_WORLD, &matSavedWorld );
    else
        matSavedWorld = *pmatWorldMatrix;

    D3DXMatrixMultiply( &matWorld, &matSavedWorld, &m_mat );
    pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // �޽��� �������� ���� ������ �������մϴ�.
    if( m_pChild )
        m_pChild->Render( pd3dDevice, TRUE, FALSE, &matWorld );

    // ���� ���� Ȱ��ȭ
    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    // �޽��� ���� ���� ������ �������մϴ�.
    if( m_pChild )
        m_pChild->Render( pd3dDevice, FALSE, TRUE, &matWorld );

    // ���� ����
    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
    pd3dDevice->SetTransform( D3DTS_WORLD, &matSavedWorld );

    return S_OK;
}