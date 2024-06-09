vs_1_1
dcl_position v0
dcl_normal v3

; position is compressed via the compressed transform 10 bit method
; Input:
;
; v0.xyz - position in the range -1.f - 1.f
; v3.xyz - normal
; c0-c3 - world view projection matrix
; c4-c7 - compression matrix
; c8 - <0.5f, 1, ?, ?>
mad r0, v0, c8.x, c8.x				; convert into 0.f to 1.f
m4x4 r1, r0, c4						; decompress position
m4x4 r0, r1, c0					; transform position

mov oPos.xyzw, r0
mov oD0.rgb, v3.xyz					; use normal data as colour

//�۾��� ����:
//1. ���ؽ��� ��ġ�� ��� �����͸� �Է����� �޽��ϴ�.
//2. ���ؽ��� ��ġ�� -1.0���� 1.0������ �������� 0.0���� 1.0������ ������ ��ȯ�մϴ�.
//3. ��ȯ�� ��ġ�� ���� �� �������� ��ķ� ��ȯ�Ͽ� ��� ��ġ�� ����մϴ�.
//4. ���ؽ��� ��� �����͸� �������� ����մϴ�.

//���������� ���캸��:
//dcl_position v0: ���ؽ��� ��ġ �����Ͱ� v0 �������Ϳ� �Էµ˴ϴ�.
//dcl_normal v3: ���ؽ��� ��� �����Ͱ� v3 �������Ϳ� �Էµ˴ϴ�.
//mad r0, v0, c8.x, c8.x: ���ؽ��� ��ġ ������(v0)�� -1.0���� 1.0������ �������� 0.0���� 1.0������ ������ ��ȯ�մϴ�. �̶�, ���(c8.x)�� ����Ͽ� ��ȯ�մϴ�.
//m4x4 r1, r0, c4: ��ȯ�� ��ġ ������(r0)�� ����� ��ȯ ���(c4)�� �������Ͽ� ���� ��ġ�� �����մϴ�.
//m4x4 r0, r1, c0: ������ ��ġ�� ���� �� �������� ���(c0)�� ��ȯ�Ͽ� ��� ��ġ(oPos)�� ����մϴ�.
//mov oPos.xyzw, r0: ���� ��� ��ġ�� ��� ��������(oPos)�� �����մϴ�.
//mov oD0.rgb, v3.xyz: ���ؽ��� ��� ������(v3.xyz)�� ���� ������ ����մϴ�. �̸� ��� ��������(oD0.rgb)�� �����մϴ�.