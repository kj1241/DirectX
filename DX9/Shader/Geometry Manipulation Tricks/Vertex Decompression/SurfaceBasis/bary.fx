// �پ��� �ﰢ���� ���� ���� ����ϱ� ���� HLSL �ڵ��Դϴ�.

//ť�� �ϳ��� �����ϴ� �ﰢ���� �����Դϴ�. 
//ť��� 6���� ������ �����Ǹ�, �� ���� 2���� �ﰢ������ �����ǹǷ� �� 12���� �ﰢ���� �ֽ��ϴ�.
#define NUM_BASE_TRIANGLE 12

//�� �ﰢ���� ���� ��ġ�� �����ϴ� �迭�Դϴ�. 
//�� �ﰢ���� 3���� �������� �����ǹǷ� �� 36���� ���� ��ġ�� �����մϴ�.
vector VertexPos[3 * NUM_BASE_TRIANGLE];
matrix MVP; //��-��-�������� ����Դϴ�. �̴� ���� ��ǥ�� Ŭ�� ��ǥ�� ��ȯ�ϴ� �� ���˴ϴ�.

struct VS_OUT
{
	float4 pos : POSITION;
	float4 col : COLOR;
};

VS_OUT mainVS( float3 vertexStream : POSITION0  ) 
{
	VS_OUT outp;
	//vertexStream�� x, y ���� �ٸ���Ʈ�� ��ǥ(barycentric coordinates)�̸�, z ���� �ﰢ�� �ε����� �Ϻ��Դϴ�.
	float i =  vertexStream.x;
	float j =  vertexStream.y;
	//�ﰢ���� �� �������� ���� ����ġ�� ��Ÿ���ϴ�. �� ������ ����Ͽ� �ﰢ�� ������ ������ ���� ����մϴ�.
	float k = 1.0 - i - j;
	float baseIndex =  vertexStream.z * 256;
	//pos�� �ٸ���Ʈ�� ��ǥ�� ����Ͽ� �ﰢ�� ������ ���� ����մϴ�.
	float3 pos =	i * VertexPos[ (baseIndex * 3) + 0 ] + 
					j * VertexPos[ (baseIndex * 3) + 1 ] + 
					k * VertexPos[ (baseIndex * 3) + 2 ];

	//outp.pos�� ��-��-�������� ����� ����Ͽ� ��ȯ�� ��ǥ�Դϴ�.
	outp.pos =  mul( float4(pos,1), MVP );
	//outp.col�� �ٸ���Ʈ�� ��ǥ�� ���� ������ �����մϴ�.
	outp.col = vector(i,j,k,1);

	return outp;
}

technique T0
{
	pass P0
	{ 
		VertexShader = compile vs_1_1 mainVS();
	}
}

//�� �ﰢ���� ���� ���� ����ϰ�, �ش� ���� ���� ��ǥ�� ��ȯ�Ͽ� �������մϴ�.
//�ٸ���Ʈ�� ��ǥ�� ����Ͽ� �ﰢ�� ������ ���� ����ϸ�, �� ��ǥ�� ���� �����ε� ����մϴ�. 
//�� ���̴��� �ַ� ť���� ���� ���� �ð�ȭ�ϴ� �� ���˴ϴ�.
