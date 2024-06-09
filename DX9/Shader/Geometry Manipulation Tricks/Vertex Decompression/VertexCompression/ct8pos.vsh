vs_1_1
dcl_position v0
dcl_normal v3

; position is compressed via the compressed transform 8 bit method
; Input:
;
; v0.xyz - position in the range 0.f - 1.f
; v3.xyz - normal
; c0-c3 - world view projection matrix
; c4-c7 - compression matrix
; c8 - <0.5f, 1, 0, 0>
m4x4 r1, v0, c4						; decompress position
m4x4 r0, r1, c0					; transform position

mov oPos.xyzw, r0
mov oD0.rgb, v3.xyz					; use normal data as colour

//�ֿ� ���:
//1. ���ؽ��� ��ġ �����Ͱ� ����Ǿ� �ֽ��ϴ�. �� �ڵ�� �ش� �����͸� �����ϰ� ��ȯ�մϴ�.
//2. ��� �����ʹ� ���� ������ ���˴ϴ�.

//������ ����
//dcl_position v0: ���ؽ��� ��ġ �����Ͱ� ����� ���·� v0 �������Ϳ� �Էµ˴ϴ�.
//dcl_normal v3: ���ؽ��� ��� �����Ͱ� v3 �������Ϳ� �Էµ˴ϴ�.
//m4x4 r1, v0, c4: ����� ��ġ �����͸� �����ϱ� ���� ���� ���� ���(c4)�� ����Ͽ� ���ؽ��� ��ġ�� �����մϴ�. ����� �ӽ� ���������� r1�� ����˴ϴ�.
//m4x4 r0, r1, c0: ���ؽ��� ��ġ �����͸� ������ ��, ���� �� �������� ���(c0)�� ����Ͽ� ��ġ�� ��ȯ�մϴ�. ����� r0�� ����˴ϴ�.
//mov oPos.xyzw, r0: ��ȯ�� ��ġ�� ��� ��ġ ��������(oPos)�� �����մϴ�.
//mov oD0.rgb, v3.xyz: ���ؽ��� ��� �����͸� �������� ����մϴ�.