#ifndef STDAFX_H // STDAFX_H�� ���� �ȵǾ� �ִٸ�
#define STDAFX_H // STDAFX_H�� ����


#include <windows.h> //winAPI
#include <string> // string����
#include <fstream>
#include <iostream>
#include <d3d12.h> //directx12 
#include <dxgi1_6.h> //dxgi
#include <D3Dcompiler.h> //d3d �����Ϸ�
#include <DirectXMath.h> //DirectX Math �Լ��� ����ϱ����ؼ�
#include "d3dx12.h"
#include <wrl.h> //Microsoft ����� ���ؼ�
#include <fstream>
#include <wincodec.h>
#include "DirectXMath.h"
#include <unordered_map>

#include <DirectXColors.h> // ���̷�Ʈ x ��
#include "DXHelper.h"


using namespace DirectX;
using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#endif //���� ��(pragma once)�� ����� ȿ��  ��������� �ߺ��Ǿ� �о �������� �������