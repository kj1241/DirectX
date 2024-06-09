////������ ��ȯ �� ���� ������ ���� HLSL �ڵ��Դϴ�.
////���̴� �ڵ�� ť���� ������ ������ ����Ͽ� ��ȯ�� ��ǥ�� ������ ����ϴ� �۾��� �����մϴ�.
////�� �ڵ�� ���� ���̴��� �����ϰ�, �� - �� - ��������(MVP) ����� ����Ͽ� �Է� ������ ��ġ�� ��ȯ�ϰ�, ������ �̿��Ͽ� ������ ����մϴ�.
//matrix MVP;
//
//// ť��� 8���� �������� �����ǹǷ�, ť���� ���� ������ �����մϴ�.
//#define NUM_BASE_VERTICES 8
//// ť���� �� ���� ��ġ�� �����ϴ� �迭�Դϴ�.
//vector VertexPos[NUM_BASE_VERTICES];
//// ť���� �� ���� ���� ���͸� �����ϴ� �迭�Դϴ�.
//vector VertexNorm[NUM_BASE_VERTICES];
//
//struct VS_IN
//{
//	float2 barycentric	: POSITION0; //barycentric: �ٸ���Ʈ�� ��ǥ�� ��Ÿ���ϴ�.
//	float3 indices		: POSITION1; //indices: �ﰢ���� �� ������ �ε����� ��Ÿ���ϴ�.
//};
//
//struct VS_OUT
//{
//	float4 pos : POSITION; //pos: ��ȯ�� ���� ��ġ��, POSITION ����ƽ�� ����Ͽ� ���� �ܰ�� ���޵˴ϴ�.
//	float4 col : COLOR; //col: ������ ��������, COLOR ����ƽ�� ����Ͽ� ���� �ܰ�� ���޵˴ϴ�.
//};
//
//VS_OUT mainVS( VS_IN vertexStream ) 
//{
//	VS_OUT outp;
//	// i, j, k�� �ٸ���Ʈ�� ��ǥ��, �ﰢ���� �� �������� ���� ����ġ�� ��Ÿ���ϴ�.
//	float i =  vertexStream.barycentric.x;
//	float j =  vertexStream.barycentric.y;
//	float k = 1.f - i - j;
//
//	//i0, i1, i2�� �� ���� �ε���
//	float i0 =  vertexStream.indices.x * 256;
//	float i1 =  vertexStream.indices.y * 256;
//	float i2 =  vertexStream.indices.z * 256;
//
//	//�� ������ ��ġ(v0, v1, v2)�� ����(n0, n1, n2)�� �迭���� �����ɴϴ�.
//	float3 v0 =	VertexPos[ i0 ];
//	float3 n0 = VertexNorm[ i0 ];
//
//	float3 v1 = VertexPos[ i1 ];
//	float3 n1 = VertexNorm[ i1 ];
//
//	float3 v2 = VertexPos[ i2 ];
//	float3 n2 = VertexNorm[ i2 ];
//
//	float3 pos, norm;
//	// ���� ��ġ
//	pos = (i * v0) + (j * v1) + (k * v2);
//	// ���� ���� 
//	norm = (i * v0) + (j * v1) + (k * v2);
//	norm = normalize( norm );
//	
//	// �̰��� ���� �������� �����ϱ� ���� ���Դϴ�. 
//	// �Ų����� ���� ������ ������� �ʽ��ϴ�.
//	// �տ��� �߻���ŵ�ϴ�. ���������� �츮�� ������ ������ �ֽ��ϴ�
//	// ��� ������ 0.3�� ��
//	pos += norm * 0.3f;
//
//	// ��� ������ ���� ���� ���� �� 0.3�� �����Ͽ� ���� �������� �̵���ŵ�ϴ�.
//	outp.pos =  mul( float4(pos,1), MVP );
//	outp.col = float4( (norm*0.5)+0.5,1);
//
//	return outp;
//}
//
//technique T0
//{
//	pass P0
//	{ 
//		VertexShader = compile vs_1_1 mainVS();
//	}
//}
//
////ȯ�� ���� ��� ���� ��꿡 ������ ���߰� �����Ǿ� �ֽ��ϴ�.


//----------------
// ���� �ڵ� ���̴� �� - 3.0
//----------------

//1. ���� �ѹ� ����
//���� �ڵ忡�� 256 ���� ���� �ѹ��� ���ǰ� �ֽ��ϴ�.
//�� ������ ��� �Ǵ� ��ũ�η� �����Ͽ� �ڵ��� �������� ���̰� ���������� ���� �� �� �ֽ��ϴ�.

//2. �ߺ� �ڵ� ����
//���� ����ü VS_IN�� VS_OUT�� �ܼ�������, �ʿ��� ��� �߰����� �����͸� �����ϵ��� Ȯ���� �� �ֽ��ϴ�. 
//���� ���, UV ��ǥ �Ǵ� �߰����� ���� �Ӽ��� ������ �� �ֽ��ϴ�.

//3. ���̴� �������� ���׷��̵�
//���� ���̴��� vs_1_1 ���������� ����ϰ� �ֽ��ϴ�. 
//�����ϴٸ�, �� �ֽ��� ���̴� ��(vs_3_0 �̻�)�� ���׷��̵��Ͽ� �� ���� ��ɰ� ����ȭ�� ����� �� �ֽ��ϴ�.

// ��Ʈ���� ����
matrix MVP;

// ť�� �ϳ��� ����մϴ� (8���� ����)
#define NUM_BASE_VERTICES 8
#define INDEX_SCALE 256

vector VertexPos[NUM_BASE_VERTICES];
vector VertexNorm[NUM_BASE_VERTICES];

struct VS_IN
{
    float2 barycentric : POSITION0;
    float3 indices     : POSITION1;
};

struct VS_OUT
{
    float4 pos : POSITION;
    float4 col : COLOR;
};

VS_OUT mainVS(VS_IN vertexStream)
{
    VS_OUT outp;

    // �ٸ���Ʈ�� ��ǥ ���
    float i = vertexStream.barycentric.x;
    float j = vertexStream.barycentric.y;
    float k = 1.f - i - j;
    int index0 = vertexStream.indices.x * INDEX_SCALE;
    int index1 = vertexStream.indices.y * INDEX_SCALE;
    int index2 = vertexStream.indices.z * INDEX_SCALE;

    // �ﰢ���� �� ������ ��ġ �� ���� ���� ���
    float3 v0 = VertexPos[index0];
    float3 n0 = VertexNorm[index0];
    float3 v1 = VertexPos[index1];
    float3 n1 = VertexNorm[index1];
    float3 v2 = VertexPos[index2];
    float3 n2 = VertexNorm[index2];

    // ���� �������� ��ġ �� ���� ���� ���
    float3 pos = (i * v0) + (j * v1) + (k * v2);
    float3 norm = normalize((i * n0) + (j * n1) + (k * n2));

    // ���� �� ����
    pos += norm * 0.3f;

    // ��ȯ�� ��ġ ���
    outp.pos = mul(float4(pos, 1), MVP);
    // ���� ���͸� ������� ���� ���
    outp.col = float4((norm * 0.5f) + 0.5f, 1);

    return outp;
}

technique T0
{
    pass P0
    {
        VertexShader = compile vs_3_0 mainVS();
    }
}
