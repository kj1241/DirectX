#pragma once

//�ؾ��� �� ���߿� ������� �ٲ�ߵ�.. ���� ������
class Camera
{
public:
	Camera();
	~Camera();

	D3DXMATRIXA16* GetViewMatrix();// ī�޶� ���
	D3DXMATRIXA16* GetBillMatrix();// ������ ���
	D3DXMATRIXA16* SetInitView(D3DXVECTOR3& pvEye, D3DXVECTOR3& pvLookat, D3DXVECTOR3& pvUp); //ī�޶� ����� �����ϱ� ���� ���Ͱ��ʱ�ȭ
	
	//ī�޶��� ��ġ
	void SetPosition(D3DXVECTOR3& pv);
	D3DXVECTOR3* GetPosition();

	//ī�޶��� �ü� ��ġ
	void SeLookatPosition(D3DXVECTOR3& pv);
	D3DXVECTOR3* GeLookatPosition();







	///// ī�޶��� ��溤�Ͱ��� �����Ѵ�.
	//void			SetUp(D3DXVECTOR3* pv) { m_vUp = *pv; }

	///// ī�޶��� ��溤�Ͱ��� ����.
	//D3DXVECTOR3* GetUp() { return &m_vUp; }

	///// ���� �����Ѵ�.
	//void			Flush() { SetView(&m_vEye, &m_vLookat, &m_vUp); }

	///// ī�޶� ��ǥ���� X������ angle��ŭ ȸ���Ѵ�.
	//D3DXMATRIXA16* RotateLocalX(float angle);

	///// ī�޶� ��ǥ���� Y������ angle��ŭ ȸ���Ѵ�.
	//D3DXMATRIXA16* RotateLocalY(float angle);

	////	D3DXMATRIXA16*	RotateLocalZ( float angle );

	//	/// ������ǥ���� *pv���� ��ġ�� �̵��Ѵ�.
	//D3DXMATRIXA16* MoveTo(D3DXVECTOR3* pv);

	///// ī�޶� ��ǥ���� X��������� dist��ŭ �����Ѵ�.(������ -dist�� ������ �ȴ�.)
	//D3DXMATRIXA16* MoveLocalX(float dist);

	///// ī�޶� ��ǥ���� Y��������� dist��ŭ �����Ѵ�.(������ -dist�� ������ �ȴ�.)
	//D3DXMATRIXA16* MoveLocalY(float dist);

	///// ī�޶� ��ǥ���� Z��������� dist��ŭ �����Ѵ�.(������ -dist�� ������ �ȴ�.)
	//D3DXMATRIXA16* MoveLocalZ(float dist);



private:

	D3DXVECTOR3 vecEye;			/// ī�޶��� ���� ��ġ
	D3DXVECTOR3 vecLookat;		/// ī�޶��� �ü� ��ġ
	D3DXVECTOR3 vecUp;			/// ī�޶��� ��溤��

	D3DXVECTOR3 vView;		/// ī�޶� ���ϴ� �������⺤��
	D3DXVECTOR3 vCross;		/// ī������ ���麤�� cross( view, up )

	D3DXMATRIXA16 matView;		/// ī�޶� ���
	D3DXMATRIXA16 matBill;		/// ������ ���(ī�޶��� �����)
};
