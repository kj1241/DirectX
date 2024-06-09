#if !defined(COMPRESSED_MESH_H_021201_DC)
#define COMPRESSED_MESH_H_021201_DC

//���� ����
class CompressedMesh
{
protected:
	LPDIRECT3DDEVICE9				m_pD3D;		//d3d ��ġ�� ���� ������
	LPDIRECT3DVERTEXBUFFER9			m_pVB;		//���ؽ� ���� �������̽��� ���� ������
	LPDIRECT3DINDEXBUFFER9			m_pIB;		//�ε��� ���� �������̽��� ���� ������
	D3DVERTEXELEMENT9				m_Decl[MAX_FVF_DECL_SIZE];	// ��Ʈ�� ����
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;		//�ε��� ���� �������̽��� ���� ������
	unsigned int				m_vertexSize;	// ���� ������ ũ��
	unsigned int				m_numVertex;	// ���� ����
	unsigned int				m_numIndices;	// �ε��� ����

	HRESULT			CopyIndexBuffer(ID3DXBaseMesh* in);
	HRESULT			DetermineScaleAndOffset(ID3DXBaseMesh* in, D3DXMATRIX& transform, D3DXVECTOR3& scale, D3DXVECTOR3& offset);
	HRESULT			DetermineRotationMatrix(ID3DXBaseMesh* in, D3DXMATRIX& matrix);
	unsigned int	SizeOfD3DVertexType(const unsigned int type);

	unsigned int				QuantiseNormal(const float nx, const float ny, const float nz);

	D3DXVECTOR3					m_soOffset;		// ������ ������ ��� Offset
	D3DXVECTOR3					m_soScale;		// ������ ������ ��� ������
	unsigned int				ScaleAndOffsetPosition8bit(const D3DXVECTOR3& in);
	void						ScaleAndOffsetPosition16bit(const D3DXVECTOR3& in, short& x, short& y, short& z, short& w);

	D3DXMATRIX					m_ctMatrix;		// ���� ��ȯ ���
	unsigned int				CompressionTransformPosition8bit(const D3DXVECTOR3& in);
	void						CompressionTransformPosition16bit(const D3DXVECTOR3& in,short& x, short& y, short& z, short& w);
	void						SlidingCompressionTransformPosition16bit(const D3DXVECTOR3& pos, const D3DXVECTOR3& norm, unsigned int* outStream);

	enum SW_AXIS
	{
		SWA_X,
		SWA_Y,
		SWA_Z
	};

	void CreateSwapMatrix(D3DXMATRIX& in, SW_AXIS xAxis, SW_AXIS yAxis, SW_AXIS zAxis);

	unsigned int CompressionTransformPositionDEC3N(const D3DXVECTOR3& in);
	unsigned int CompressionTransformPosition101012(const D3DXVECTOR3& in);

public:
	CompressedMesh(LPDIRECT3DDEVICE9 pD3D);	// �⺻ ctor
	~CompressedMesh();							// dtor

	// D3XMesh�� ������ ������ �� ����� �޽÷� ����ȭ�մϴ�.
	HRESULT QuantiseNormals(ID3DXBaseMesh* in);

	// D3XMesh�� ������ �� ����� �޽ÿ� ������ �� ������(8��Ʈ)�� �����մϴ�.
	HRESULT ScaleAndOffsetPosition8bit(ID3DXBaseMesh* in);
	HRESULT ScaleAndOffsetPosition16bit(ID3DXBaseMesh* in);

	HRESULT CompressionTransformPosition8bit(ID3DXBaseMesh* in);
	HRESULT CompressionTransformPosition16bit(ID3DXBaseMesh* in);
	HRESULT SlidingCompressionTransformPosition16bit(ID3DXBaseMesh* in);

	// Dx9�� ���ο� ��
	HRESULT CompressionTransformPositionDEC3N(ID3DXBaseMesh* in);
	HRESULT CompressionTransformPosition101012(ID3DXBaseMesh* in);

	HRESULT Draw();	// D3XMesh�� ���� ������ �׸��� �Ͱ� �����մϴ�.

	LPDIRECT3DVERTEXBUFFER9	GetVertexBuffer() { return m_pVB; };
	LPDIRECT3DINDEXBUFFER9	GetIndexBuffer() { return m_pIB; };
	D3DVERTEXELEMENT9* GetStreamDeclaration() { return m_Decl; };
	unsigned int			GetVertexSize() const { return m_vertexSize; };

	D3DXVECTOR3& GetScaleOffsetScale() { return m_soScale; };
	D3DXVECTOR3& GetScaleOffsetOffset() { return m_soOffset; };
	D3DXMATRIX& GetCompressionTransfromMatrix() { return m_ctMatrix; };
};
#endif