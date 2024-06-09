////------------------
////���̴� �� 3
////1.�Է� �� ��� ����ü Ȯ��
////2.�ؽ�ó ���� �߰�
////3.���� ��� �߰�
////------------------
//
////�߰��� ��� ����
////1.�Է� �� ��� ����ü Ȯ��
////  - VS_IN ����ü�� texcoord �ʵ带 �߰��Ͽ� �ؽ�ó ��ǥ�� �����մϴ�.
////  - VS_OUT ����ü�� texcoord, norm, worldPos �ʵ带 �߰��Ͽ� �ؽ�ó ��ǥ, ������ ����, ���� ��ǥ�� �����մϴ�.
//// 
////2.�ؽ�ó ���� �߰�
////  - TextureSampler ���÷��� �����Ͽ� �ؽ�ó ���ø��� �߰��մϴ�.
////  - �ؽ�ó ��ǥ�� VS_OUT ����ü�� �����ϰ�, �ȼ� ���̴����� �ؽ�ó ������ ���ø��մϴ�.
////
////3.���� ��� �߰�
////  - �𷺼ų� ����Ʈ ����(LightDirection), ����(LightColor), �ں��Ʈ ����(AmbientColor)�� �߰��մϴ�.
////  - �ȼ� ���̴����� ���� ����� �����մϴ�.�븻 ���Ϳ� ����Ʈ ������ ����(dot product)�� ����Ͽ� Ȯ�� ����(diffuse lighting)�� �����մϴ�.
////  - ���� ������ �ؽ�ó ����� ���� ������ ������ ���˴ϴ�.
////
////4.�׶��̼� �߰�
////  - gradientFactor�� ���� ��ǥ�� Y ��ġ�� ������� ���˴ϴ�. ���⼭ Y ��ǥ�� 10���� ������ ����ȭ�ϰ�, saturate �Լ��� ����Ͽ� 0�� 1 ���̷� Ŭ�����մϴ�. �ʿ信 ���� �׶��̼� ������ ������ �� �ֽ��ϴ�.
////  - gradientColor�� lerp �Լ��� ����Ͽ� ���� (1, 0, 0, 1)�� �Ķ� (0, 0, 1, 1) ���̸� �����մϴ�.
////  - ���� ����(finalColor)�� �׶��̼� ����(gradientColor)�� ���Ͽ� ��� ����(outp.col)�� ����մϴ�.
//
//matrix MVP;
//matrix Model;
//matrix View;
//matrix Projection;
//
//#define NUM_CONSTS 20
//#define NUM_BASE_VERTICES (NUM_CONSTS-4) / 2
//#define MAX_DISPLACEMENT_HEIGHT  1
//
//vector VertexPos[NUM_BASE_VERTICES];
//vector VertexNorm[NUM_BASE_VERTICES];
//
//struct VS_IN
//{
//    float2 barycentric : POSITION0;
//    float4 indices_disp : POSITION1;
//    float2 texcoord : TEXCOORD0;
//};
//
//struct VS_OUT
//{
//    float4 pos : POSITION;
//    float4 col : COLOR;
//    float2 texcoord : TEXCOORD0;
//    float3 norm : TEXCOORD1;
//    float3 worldPos : TEXCOORD2;
//};
//
//VS_OUT mainVS(VS_IN vertexStream)
//{
//    VS_OUT outp;
//
//    float i = vertexStream.barycentric.x;
//    float j = vertexStream.barycentric.y;
//    float k = 1.f - i - j;
//    float i0 = vertexStream.indices_disp.x * 256;
//    float i1 = vertexStream.indices_disp.y * 256;
//    float i2 = vertexStream.indices_disp.z * 256;
//    float displace = vertexStream.indices_disp.w * MAX_DISPLACEMENT_HEIGHT;
//
//    // �� �ﰢ���� ��ġ�� ����
//    float3 v0 = VertexPos[i0];
//    float3 n0 = VertexNorm[i0];
//    float3 v1 = VertexPos[i1];
//    float3 n1 = VertexNorm[i1];
//    float3 v2 = VertexPos[i2];
//    float3 n2 = VertexNorm[i2];
//    float3 pos, norm;
//
//    // ���� ��ġ
//    pos = (i * v0) + (j * v1) + (k * v2);
//
//    // ���� ����
//    norm = (i * n0) + (j * n1) + (k * n2);
//    norm = normalize(norm);
//
//    // ������ ���� ����
//    pos += norm * displace;
//
//    // ��ġ ��ȯ
//    float4 worldPos = float4(pos, 1);
//    outp.pos = mul(worldPos, MVP);
//    outp.worldPos = pos;
//
//    // texcoord�� ����մϴ�.
//    outp.texcoord = vertexStream.texcoord;
//
//    // �Ϲ� ���
//    outp.norm = norm;
//
//    // ���� ���
//    outp.col = float4((norm * 0.5) + 0.5, 1);
//
//    return outp;
//}
//
//struct PS_IN
//{
//    float4 pos : POSITION;
//    float4 col : COLOR;
//    float2 texcoord : TEXCOORD0;
//    float3 norm : TEXCOORD1;
//    float3 worldPos : TEXCOORD2;
//};
//
//struct PS_OUT
//{
//    float4 col : COLOR;
//};
//
//sampler2D TextureSampler : register(s0);
//float3 LightDirection;
//float4 LightColor;
//float4 AmbientColor;
//
//PS_OUT mainPS(PS_IN frag)
//{
//    PS_OUT outp;
//
//    // �ؽ�ó ���ø�
//    float4 texColor = tex2D(TextureSampler, frag.texcoord);
//
//    // ���� ���
//    float3 normal = normalize(frag.norm);
//    float3 lightDir = normalize(LightDirection);
//    float NdotL = max(dot(normal, lightDir), 0.0);
//
//    float4 diffuse = LightColor * NdotL;
//    float4 ambient = AmbientColor;
//
//    // �ؽ�ó ����� ���� ����
//    float4 finalColor = texColor * (diffuse + ambient);
//
//    // ���� Y ��ġ�� ���� ������-�Ķ��� �׶��̼�
//    float gradientFactor = saturate(frag.worldPos.y / 10.0); // �׷����Ʈ ������ �����ϱ� ���� ������ �����մϴ�.
//    float4 gradientColor = lerp(float4(1, 0, 0, 1), float4(0, 0, 1, 1), gradientFactor);
//
//    // ���� ������ �׶���Ʈ�� ����
//    outp.col = finalColor * frag.col * gradientColor;
//
//    return outp;
//}
//
//technique T0
//{
//    pass P0
//    {
//        VertexShader = compile vs_3_0 mainVS();
//        PixelShader = compile ps_3_0 mainPS();
//    }
//}


//------------------
//���̴� �� 1.1 
//------------------

//������ ��ȯ �� ���� ������ ���� HLSL �ڵ��Դϴ�.
matrix MVP;
//MVP�� ����, ��, �������� ��Ʈ������ ��������, ��ȯ�� ���� ���˴ϴ�.

// ������ ���� 2�� - ����� ��� 4��
// �׷��� ����. �츮�� �̰��� D3DXMACRO�� ���� �ڵ忡�� ������ ���� �����մϴ�.
//NUM_CONSTS�� ��� ������ ����� �� ������ �����մϴ�.
//NUM_BASE_VERTICES�� ����� �⺻ ������ ���� �����մϴ�.
//MAX_DISPLACEMENT_HEIGHT�� ���� ������ �ִ� ���̸� �����մϴ�.
//VertexPos�� VertexNorm�� ���� ������ ��ġ�� ������ �����ϴ� �迭�Դϴ�.
#define NUM_CONSTS 20
#define NUM_BASE_VERTICES (NUM_CONSTS-4 ) / 2
#define MAX_DISPLACEMENT_HEIGHT  1

vector VertexPos[NUM_BASE_VERTICES];
vector VertexNorm[NUM_BASE_VERTICES];

//VS_IN�� ���� ���̴��� �Է� ����ü��, �ٸ����� ��ǥ(barycentric)�� �ε��� �� ����(indices_disp)�� �����մϴ�.
//VS_OUT�� ���� ���̴��� ��� ����ü��, ��ȯ�� ��ġ(pos)�� ����(col)�� �����մϴ�.
struct VS_IN
{
    float2 barycentric	: POSITION0;
    float4 indices_disp	: POSITION1;
};

struct VS_OUT
{
    float4 pos : POSITION;
    float4 col : COLOR;
};


//�ٸ����� ��ǥ(i, j, k)�� ����Ͽ� ��ġ�� ������ ���� �����մϴ�.
//���� ���� ���� �������� �����Ͽ� ������ ��ġ�� ����մϴ�.
//������ ��ġ�� MVP ��Ʈ������ ����� ��ȯ�մϴ�.
//���� ���͸� �̿��� ������ ����մϴ�.
VS_OUT mainVS(VS_IN vertexStream)
{
    VS_OUT outp;

    float i = vertexStream.barycentric.x;
    float j = vertexStream.barycentric.y;
    float k = 1.f - i - j;
    float i0 = vertexStream.indices_disp.x * 256;
    float i1 = vertexStream.indices_disp.y * 256;
    float i2 = vertexStream.indices_disp.z * 256;
    float displace = vertexStream.indices_disp.w * MAX_DISPLACEMENT_HEIGHT;

    // �� �ﰢ���� ��ġ�� ����
    float3 v0 = VertexPos[i0];
    float3 n0 = VertexNorm[i0];
    float3 v1 = VertexPos[i1];
    float3 n1 = VertexNorm[i1];
    float3 v2 = VertexPos[i2];
    float3 n2 = VertexNorm[i2];
    float3 pos, norm;

    // ���� ��ġ
    pos = (i * v0) + (j * v1) + (k * v2);

    // ���� ����
    norm = (i * v0) + (j * v1) + (k * v2);
    norm = normalize(norm);

    // ������ ���� ����
    pos += norm * displace;

    // ��ġ ��ȯ
    outp.pos = mul(float4(pos, 1), MVP);
    outp.col = float4((norm * 0.5) + 0.5, 1);
    //	outp.col = displace;

    return outp;
}

//T0 ����� P0 �н��� �����մϴ�.
//P0 �н������� mainVS ���� ���̴��� �������Ͽ� ����մϴ�.
technique T0
{
    pass P0
    {
        VertexShader = compile vs_1_1 mainVS();
    }
}
