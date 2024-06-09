vs_1_1
dcl_position v0
dcl_normal v3
; Input:
;
; v0.xyz - position
; v3.xyz - normal mapped -1.f -> -1.f, 1.f -> 1.f
; c0-c3 - world view projection matrix
; c4 - <0.5.f, 0.f, 0, 0>
m4x4 oPos, v0, c0					; transform position
mad oD0.rgb, v3.xyz, c4.x, c4.x		; use normal data as colour

//�۾��� ����:
//1. ���ؽ��� ��ġ�� ��� �����͸� �Է����� �޽��ϴ�.
//2. ���ؽ��� ��ġ�� ���� �� �������� ��ķ� ��ȯ�Ͽ� ��� ��ġ�� ����մϴ�.
//3. ���ؽ��� ��� �����͸� �������� ����մϴ�.

//������ ����:
//dcl_position v0: ���ؽ��� ��ġ �����Ͱ� v0 �������Ϳ� �Էµ˴ϴ�.
//dcl_normal v3: ���ؽ��� ��� �����Ͱ� v3 �������Ϳ� �Էµ˴ϴ�.
//m4x4 oPos, v0, c0: ���ؽ��� ��ġ ������(v0)�� ���� �� �������� ���(c0)�� ��ȯ�Ͽ� ��� ��ġ(oPos)�� ����մϴ�.
//mad oD0.rgb, v3.xyz, c4.x, c4.x: ���ؽ��� ��� ������(v3.xyz)�� ���� ������ ����մϴ�. ��� �������� x, y, z ���� ���(c4.x)�� ���Ͽ� �������� ����ϰ�, ����� oD0.rgb�� �����մϴ�.