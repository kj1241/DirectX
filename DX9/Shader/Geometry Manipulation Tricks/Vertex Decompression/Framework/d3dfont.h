#ifndef D3DFONT_H
#define D3DFONT_H
#include <tchar.h>
#include <D3D9.h>

// �۲� ���� �÷���
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_ZENABLE     0x0004

// �۲� ������ �÷���
#define D3DFONT_CENTERED_X  0x0001
#define D3DFONT_CENTERED_Y  0x0002
#define D3DFONT_TWOSIDED    0x0004
#define D3DFONT_FILTERED    0x0008

class CD3DFont
{
    TCHAR   m_strFontName[80];    // �۲� �Ӽ�
    DWORD   m_dwFontHeight;
    DWORD   m_dwFontFlags;

    LPDIRECT3DDEVICE9       m_pd3dDevice; // �������� ���Ǵ� D3DDevice
    LPDIRECT3DTEXTURE9      m_pTexture;   // �� �۲��� d3d �ؽ�ó
    LPDIRECT3DVERTEXBUFFER9 m_pVB;        // �ؽ�Ʈ �������� ���� VertexBuffer
    DWORD   m_dwTexWidth;                 // �ؽ�ó ũ��
    DWORD   m_dwTexHeight;
    FLOAT   m_fTextScale;
    FLOAT   m_fTexCoords[128-32][4];
    DWORD   m_dwSpacing;                  // ����� ���� �ȼ� ����

    // ���� ���� ���� �� ������ ���� ���� ���
    LPDIRECT3DSTATEBLOCK9 m_pStateBlockSaved;
    LPDIRECT3DSTATEBLOCK9 m_pStateBlockDrawText;

public:
    // 2D �� 3D �ؽ�Ʈ �׸��� ���
    HRESULT DrawText( FLOAT x, FLOAT y, DWORD dwColor, 
                      const TCHAR* strText, DWORD dwFlags=0L );
    HRESULT DrawTextScaled( FLOAT x, FLOAT y, FLOAT z, 
                            FLOAT fXScale, FLOAT fYScale, DWORD dwColor, 
                            const TCHAR* strText, DWORD dwFlags=0L );
    HRESULT Render3DText( const TCHAR* strText, DWORD dwFlags=0L );
    
    // �ؽ�Ʈ�� ������ �������� �Լ�
    HRESULT GetTextExtent( const TCHAR* strText, SIZE* pSize );

    // ��ġ ���� ��ü �ʱ�ȭ �� ����
    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();
    HRESULT DeleteDeviceObjects();

    // ������ / �Ҹ���
    CD3DFont( const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags=0L );
    ~CD3DFont();
};
#endif


