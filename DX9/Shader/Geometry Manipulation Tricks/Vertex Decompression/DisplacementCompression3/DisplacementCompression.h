#pragma once

// TODO: "DirectX AppWizard Apps"�� ����� �̸��̳� ȸ�� �̸����� �����ϼ���
#define DXAPP_KEY        TEXT("Software\\DirectX9\\SurfaceBasis")

// ���� �Է� ���¸� �����ϴ� ����ü
struct UserInput
{
    BYTE diks[256];   // DirectInput Ű���� ���� ���� 
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;

    BOOL b1, b2, b3, b4, b5, b6, b7, b8, b9;
    BOOL bf5, bf6, bf7, bf8, bf9;
};

enum DISPLAY_METHOD
{
    DM_LINEAR,
};

class CMyD3DApplication : public CD3DApplication
{
    BOOL m_bLoadingApp;          // ���ø����̼��� �ε� ������ ����
    CD3DFont* m_pFont;           // �ؽ�Ʈ�� �׸��� ���� ��Ʈ
    ID3DXMesh* m_pD3DXMesh;      // ������(Teapot)�� �����ϱ� ���� D3DX �޽�
    ID3DXPatchMesh* m_pD3DXPatchMesh; // ������(Teapot)�� �����ϱ� ���� D3DX ��ġ �޽�
    ID3DXMesh* m_pD3DXPatchMeshDest;  // ������(Teapot)�� �����ϱ� ���� D3DX ��ġ �޽�
    LPDIRECTINPUT8 m_pDI;        // DirectInput ��ü
    LPDIRECTINPUTDEVICE8 m_pKeyboard; // DirectInput Ű���� ��ġ
    UserInput m_UserInput;       // ����� �Է��� �����ϴ� ����ü 
    FLOAT m_fWorldRotX;          // ���� ȸ�� ���� X��
    FLOAT m_fWorldRotY;          // ���� ȸ�� ���� Y��
    LPD3DXEFFECT m_linearFX;     // npatch ȿ��

    unsigned int			m_numVertices;
    unsigned int			m_numIndices;
    float* m_posVertexData;
    float* m_normVertexData;
    float* m_uvVertexData;
    WORD* m_indexData;

    LPDIRECT3DVERTEXBUFFER9 m_pVB; // ������ �������� VB
    LPDIRECT3DINDEXBUFFER9 m_pIB;  // ������ �������� IB (����� ���)

    D3DXMATRIX				m_matProj;
    D3DXMATRIX				m_matView;
    D3DXMATRIX				m_matWorld;
    DISPLAY_METHOD			m_displayMethod;

    HRESULT SetupDisplayMethod();
    void CleanDisplayMethod();
    HRESULT GetXMeshData(LPD3DXMESH in);


    // �׼������� ���� ����
    unsigned int			m_lod;
    float LookupDisplacementValue(WORD i0, WORD i1, WORD i2, float i, float j);
    float Fade(float t);
    float Lerp(float t, float a, float b);
    float Grad(int hash, float x, float y);
    void InitPermutation();
    float PerlinNoise(float x, float y);
    void  GeneratePerlinNoisePattern(DWORD dwSize);
    //void CreateMandelbrot(unsigned int dwSize);
    float* m_DisplacementTexture;

    // ���� ǥ�� ����
    LPDIRECT3DVERTEXDECLARATION9	m_pLinearDecl;
    float* m_linearPos;
    float* m_linearNorm;
    HRESULT							GenerateLinearConstants();
    HRESULT							GenerateLinearBuffers(unsigned int LOD);

    LPDIRECT3DPIXELSHADER9 m_pPixelShader;
    LPD3DXCONSTANTTABLE m_pConstantTable;

    int p[512];
    int permutation[256] = {
        151, 160, 137, 91, 90, 15, 131, 113, 197, 224, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
            190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87,
            174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211,
            133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208,
            89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
            5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213,
            119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113,
            224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145,
            235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138,
            236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
    };


protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice(D3DCAPS9*, DWORD, D3DFORMAT);

    HRESULT RenderText();

    HRESULT InitInput(HWND hWnd);
    void    UpdateInput(UserInput* pUserInput);
    void    CleanupDirectInput();
    VOID    ReadSettings();
    VOID    WriteSettings();

public:
    LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};