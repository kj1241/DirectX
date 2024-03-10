#include "stdafx.h"
#include "DirectX12Pipline.h"
#include "WinAPI.h"


DirectX12Pipline::DirectX12Pipline() : pD3d12Device(nullptr), pCommandQueue(nullptr), pSwapChain(nullptr), pRtvHeap(nullptr), pRenderTargets{ nullptr }, pCommandAllocator(nullptr), pCommandList(nullptr), pFence(nullptr), hFenceEvent(nullptr), pPipelineState(nullptr), 
pRootSignature(nullptr), pVertexBuffer(nullptr), pIndexBuffer(nullptr),
pSrvHeap(nullptr),
pTexture(nullptr),
viewport(0.0f, 0.0f, 0.0f, 0.0f),
scissorRect(0, 0, 0, 0),
result(0), frameIndex(0), rtvDescriptorSize(0), fenceValue(0)
{
	GetAssetsPath(assetsPath, _countof(assetsPath));
}

DirectX12Pipline::~DirectX12Pipline()
{
}

void DirectX12Pipline::OnInit()
{
	InitSize();
	LoadPipeline();
	LoadAssets();
}

void DirectX12Pipline::OnUpdate()
{
}

// ����� ������
void DirectX12Pipline::OnRender()
{
	// ����� �������ϴµ� �ʿ��� ��� Ŀ��带 Ŀ��帮��Ʈ�� �����
	PopulateCommandList();

	// Ŀ��帮��Ʈ�� ����
	ID3D12CommandList* ppCommandLists[] = { pCommandList };
	pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// �������� ����
	result = pSwapChain->Present(1, 0);
	if (result < 0)
		return;

	WaitForPreviousFrame();
}

void DirectX12Pipline::OnDestroy()
{
	// GPU�� �� �̻� ������ ���ҽ��� �������� �ʴ��� Ȯ��.
   // �Ҹ��ڿ� ���� ����.
	WaitForPreviousFrame();
	
	CloseHandle(hFenceEvent);

	if (pFence != nullptr)
	{
		pFence->Release();
		pFence = nullptr;
	}

	if (pTexture != nullptr)
	{
		pTexture->Release();
		pTexture = nullptr;
	}
	
	if (pIndexBuffer != nullptr)
	{
		pIndexBuffer->Release();
		pIndexBuffer = nullptr;
	}

	if (pVertexBuffer != nullptr)
	{
		pVertexBuffer->Release();
		pVertexBuffer= nullptr;
	}
	
	if (pCommandList != nullptr)
	{
		pCommandList->Release();
		pCommandList = nullptr;
	}

	for (int i = 0; i < FrameCount; ++i)
		if (pRenderTargets[i] != nullptr)
		{
			pRenderTargets[i]->Release();
			pRenderTargets[i] = nullptr;
		}

	if (pCommandAllocator != nullptr)
	{
		pCommandAllocator->Release();
		pCommandAllocator = nullptr;
	}

	if (pRootSignature != nullptr)
	{
		pRootSignature->Release();
		pRootSignature = nullptr;
	}

	if (pSrvHeap != nullptr)
	{
		pSrvHeap->Release();
		pSrvHeap = nullptr;
	}
	
	if (pRtvHeap != nullptr)
	{
		pRtvHeap->Release();
		pRtvHeap = nullptr;
	}

	if (pSwapChain != nullptr)
	{
		pSwapChain->Release();
		pSwapChain = nullptr;
	}

	if (pCommandQueue != nullptr)
	{
		pCommandQueue->Release();
		pCommandQueue = nullptr;
	}

	if (pD3d12Device != nullptr)
	{
		pD3d12Device->Release();
		pD3d12Device = nullptr;
	}

	delete imageData;
}

//�����ڿ��� �ʱⰪ�� �Ҵ����ټ� ���ºκе�.(������ ���ؼ�)
void DirectX12Pipline::InitSize()
{
	width = WinAPI::pWinAPI->GetWidth();
	height = WinAPI::pWinAPI->GetHeigth();
	viewport.Width = width;
	viewport.Height = height;
	scissorRect.right = width;
	scissorRect.bottom = height;
	aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

// ������ ���������� �ε�
void DirectX12Pipline::LoadPipeline()
{
	/// <summary>
	/// ����� ���̾ Ȱ��ȭ�մϴ�(�׷��� ������ "������ ���" �ʿ�).
	/// ����: ��ġ ���� �� ����� ������ Ȱ��ȭ�ϸ� Ȱ�� ��ġ�� ��ȿȭ�˴ϴ�.
	/// </summary>
	UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
	{
		ID3D12Debug* pDebugController = nullptr;
		result = D3D12GetDebugInterface(__uuidof(*pDebugController), (void**)&pDebugController);
		if (result)
		{
			pDebugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG; // �߰� ����� ���̾ Ȱ��ȭ�մϴ�.
		}
		pDebugController->Release();
		pDebugController = nullptr;
	}
#endif

	/// <summary>
	/// �ϵ���� �׷���ī�� ��� ã��
	/// </summary>
	IDXGIFactory4* pFactory = nullptr;
	result = CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(*pFactory), (void**)&pFactory);
	if (result < 0)
		return;

	if (useWarpDevice)
	{
		IDXGIAdapter* pWarpAdapter = nullptr;
		//���� ������ ��� ã��
		pFactory->EnumWarpAdapter(__uuidof(*pWarpAdapter), (void**)&pWarpAdapter);
		result = D3D12CreateDevice(pWarpAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(*pD3d12Device), (void**)&pD3d12Device);
		if (result < 0)
			return;

		pWarpAdapter->Release();
		pWarpAdapter = nullptr;

	}
	else
	{
		IDXGIAdapter1* pHardwareAdapter = nullptr;
		//�ϵ���� ��� ã��
		GetHardwareAdapter(pFactory, &pHardwareAdapter);
		result = D3D12CreateDevice(pHardwareAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(*pD3d12Device), (void**)&pD3d12Device);
		if (result < 0)
			return;

		pHardwareAdapter->Release();
		pHardwareAdapter = nullptr;
	}

	/// <summary>
	/// Ŀ��� ť�� �����ϰ� ����
	/// </summary>
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.NodeMask = 0;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

		result = pD3d12Device->CreateCommandQueue(&queueDesc, __uuidof(*pCommandQueue), (void**)&pCommandQueue);
		if (result < 0)
			return;
	}

	/// <summary>
	/// ����ü��(�����)�� �����ϰ� ����
	/// </summary>
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		swapChainDesc.BufferCount = FrameCount;
		swapChainDesc.Width = WinAPI::pWinAPI->GetWidth();
		swapChainDesc.Height = WinAPI::pWinAPI->GetHeigth();
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Stereo = false;

		IDXGISwapChain1* tempSwapChain;
		result = pFactory->CreateSwapChainForHwnd(
			pCommandQueue,        //�������� �ݵ�� �ϼ��Ǹ� ���Լ����ؾߵ����� ť�� �o�Ź۾Ƽ� ��⿭�� �����.
			WinAPI::pWinAPI->GetHwnd(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&tempSwapChain
		);
		if (result < 0)
			return;

		//��üȭ�� ���� �̰� ���⿡ ���̴� ������ ����ü�� �׸��� ȭ���� ��� �׷��ٰ����� �����ؾ� �Ǳ� ������.
		result = pFactory->MakeWindowAssociation(WinAPI::pWinAPI->GetHwnd(), DXGI_MWA_NO_ALT_ENTER);
		if (result < 0)
			return;

		//�̷� ������ �ۼ��� ����? ����ü�� ���� �ٲٱ� ���ؼ�
		//tempSwapChain.As(&pSwapChain);
		result = tempSwapChain->QueryInterface(__uuidof(pSwapChain), (void**)&pSwapChain);
		if (result < 0)
			return;

		//����� ��ȣ��������
		frameIndex = pSwapChain->GetCurrentBackBufferIndex();

		tempSwapChain->Release();
		tempSwapChain = nullptr;
	}

	/// <summary>
	/// ���� ����
	/// </summary> 
	{
		//RTV(������ ��� ��) ������ ���� �����ϰ� ����
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		result = pD3d12Device->CreateDescriptorHeap(&rtvHeapDesc, __uuidof(*pRtvHeap), (void**)&pRtvHeap); //������ ���°�
		if (result < 0)
			return;

		//�ؽ��ĸ� ����ؾߵǱ⶧���� SRV�� ����

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		result = pD3d12Device->CreateDescriptorHeap(&srvHeapDesc, __uuidof(*pSrvHeap), (void**)&pSrvHeap);
		if (result < 0)
			return;

		rtvDescriptorSize = pD3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// ������ ���ҽ� ����
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pRtvHeap->GetCPUDescriptorHandleForHeapStart());

		// RTV���� ������ ����
		for (UINT i = 0; i < FrameCount; ++i)
		{
			result = pSwapChain->GetBuffer(i, __uuidof(*pRenderTargets[i]), (void**)&pRenderTargets[i]);
			pD3d12Device->CreateRenderTargetView(pRenderTargets[i], nullptr, rtvHandle);
			rtvHandle.Offset(1, rtvDescriptorSize);
		}
	}

	/// <summary>
	/// Ŀ��� ����Ʈ�� �Ҵ��Ѵ�.
	/// </summary>
	{
		result = pD3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(*pCommandAllocator), (void**)&pCommandAllocator);
		if (result < 0)
			return;

	}
	pFactory->Release();
	pFactory = nullptr;
}

//������ �ε�
void DirectX12Pipline::LoadAssets()
{

	/// <summary>
	/// ��Ʈ�ñ׳��� ����
	/// </summary>
	{
		ID3DBlob* pSignature = nullptr;
		ID3DBlob* pError = nullptr;

		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1; //�ϴ� ��Ʈ �ñ״��� ���� 1.1�� �ۼ��ϰ�

		result = pD3d12Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)); //�����ϴ��� Ȯ������
		if (result < 0) //�������� ������
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0; // ���� 1.0���� ������
			D3D12_DESCRIPTOR_RANGE ranges[1] = {};
			ranges[0].NumDescriptors = 1;
			ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			ranges[0].BaseShaderRegister = 0;
			ranges[0].RegisterSpace = 0;
			ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_PARAMETER rootParameters[1] = {};
			rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[0].DescriptorTable.pDescriptorRanges = &ranges[0];
			rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
			rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			D3D12_STATIC_SAMPLER_DESC sampler = {};
			sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler.MipLODBias = 0;
			sampler.MaxAnisotropy = 0;
			sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			sampler.MinLOD = 0.0f;
			sampler.MaxLOD = D3D12_FLOAT32_MAX;
			sampler.ShaderRegister = 0;
			sampler.RegisterSpace = 0;
			sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
			rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			rootSignatureDesc.NumParameters = _countof(rootParameters);
			rootSignatureDesc.pParameters = rootParameters;
			rootSignatureDesc.NumStaticSamplers = 1;
			rootSignatureDesc.pStaticSamplers = &sampler;

			result = D3D12SerializeRootSignature(&rootSignatureDesc, featureData.HighestVersion, &pSignature, &pError);
			if (result < 0)
				return;

			result = pD3d12Device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), __uuidof(*pRootSignature), (void**)&pRootSignature);
			if (result < 0)
				return;

		}
		else //�����ϸ�
		{


			D3D12_DESCRIPTOR_RANGE1 ranges[1] = {};
			ranges[0].NumDescriptors = 1;
			ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			ranges[0].BaseShaderRegister = 0;
			ranges[0].RegisterSpace = 0;
			ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_PARAMETER1 rootParameters[1] = {};
			rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParameters[0].DescriptorTable.pDescriptorRanges = &ranges[0];
			rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
			rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			//rootParameters[0].Descriptor = CBV1rootDescriptor;

			D3D12_STATIC_SAMPLER_DESC sampler = {};
			sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
			sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
			sampler.MipLODBias = 0;
			sampler.MaxAnisotropy = 0;
			sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
			sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
			sampler.MinLOD = 0.0f;
			sampler.MaxLOD = D3D12_FLOAT32_MAX;
			sampler.ShaderRegister = 0;
			sampler.RegisterSpace = 0;
			sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

			D3D12_ROOT_SIGNATURE_DESC1 rootSignatureDesc = {};
			rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			rootSignatureDesc.NumParameters = _countof(rootParameters);
			rootSignatureDesc.pParameters = rootParameters;
			rootSignatureDesc.NumStaticSamplers = 1;
			rootSignatureDesc.pStaticSamplers = &sampler;


			D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionRootSignatureDesc = {};
			versionRootSignatureDesc.Desc_1_1 = rootSignatureDesc;
			versionRootSignatureDesc.Version = featureData.HighestVersion;

			result = D3D12SerializeVersionedRootSignature(&versionRootSignatureDesc, &pSignature, &pError);
			if (result < 0)
				return;


			result = pD3d12Device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), __uuidof(*pRootSignature), (void**)&pRootSignature);
			if (result < 0)
				return;
		}

		if (pSignature != nullptr)
		{
			pSignature->Release();
			pSignature = nullptr;
		}
		if (pError != nullptr)
		{
			pError->Release();
			pError = nullptr;
		}

	}

	/// <summary>
	/// ���� ������������ ����� ���̴� ������ �� �ε�
	/// </summary>
	{
		ID3DBlob* vertexShader = nullptr;
		ID3DBlob* pixelShader = nullptr;


	//����� ��带 ����Ͽ� ���̴� �����
	#if defined(_DEBUG)
		UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	#else
		UINT compileFlags = 0;
	#endif

		wchar_t assetPath[512] = {};
		GetAssetFullPath(L"../../BasicVertexShader.hlsl", assetPath);
		result = D3DCompileFromFile(assetPath, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
		if (result < 0)
			return;

		GetAssetFullPath(L"../../BasicPixelShader.hlsl", assetPath);
		result = D3DCompileFromFile(assetPath, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);
		if (result < 0)
			return;

		// ���� ���̾ƿ��� ����
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
		renderTargetBlendDesc.BlendEnable = false;
		renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		renderTargetBlendDesc.LogicOpEnable = false;

		//�׷��� ���� ����������(PSO)���� �� ����
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = pRootSignature;
		//psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader);
		psoDesc.VS.BytecodeLength = vertexShader->GetBufferSize();
		psoDesc.VS.pShaderBytecode = vertexShader->GetBufferPointer();
		//psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader);
		psoDesc.PS.BytecodeLength = pixelShader->GetBufferSize();
		psoDesc.PS.pShaderBytecode = pixelShader->GetBufferPointer();
		//psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT)
		psoDesc.RasterizerState.MultisampleEnable = false;
		psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		psoDesc.RasterizerState.DepthClipEnable = true;

		psoDesc.RasterizerState.FrontCounterClockwise = false;
		psoDesc.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		psoDesc.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		psoDesc.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		psoDesc.RasterizerState.AntialiasedLineEnable = false;
		psoDesc.RasterizerState.ForcedSampleCount = 0;
		psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		//psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.BlendState.AlphaToCoverageEnable = false;
		psoDesc.BlendState.IndependentBlendEnable = false;
		psoDesc.BlendState.RenderTarget[0] = renderTargetBlendDesc;

		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;
		result = pD3d12Device->CreateGraphicsPipelineState(&psoDesc, __uuidof(*pPipelineState), (void**)&pPipelineState);
		if (result < 0)
			return;

		if (vertexShader != nullptr)
		{
			vertexShader->Release();
			vertexShader = nullptr;
		}
		if (pixelShader != nullptr)
		{
			pixelShader->Release();
			pixelShader = nullptr;
		}

	}

	/// <summary>
	///Ŀ��� ����Ʈ�� ����
	/// </summary>
	{
		result = pD3d12Device->CreateCommandList(0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			pCommandAllocator,
			pPipelineState,
			__uuidof(*pCommandList), (void**)&pCommandList);
		if (result < 0)
			return;
		////Ŀ��� ����Ʈ�� �� �̻� �� ���� ������ �ݾ� ���´�. 
		//result = pCommandList->Close();
		//if (result < 0)
		//	return;
	}

	/// <summary>
	/// ���ý� ���� �����
	/// </summary>
	{

		Vertex triangleVertices[] =
		{
			{ { -0.25f, 0.25f * aspectRatio, 0.0f }, { 0.0f, 0.0f } },
			{ { 0.25f, -0.25f * aspectRatio, 0.0f }, { 1.0f, 1.0f } },
			{ { -0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 1.0f } },
			{ { 0.25f, 0.25f * aspectRatio, 0.0f }, { 1.0f, 0.0f } }
		};

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = sizeof(triangleVertices);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		result = pD3d12Device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(*pVertexBuffer), (void**)&pVertexBuffer);
		if (result < 0)
			return;


		//�ﰢ�� ���������͸� ���ۿ� ����
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);        // CPU���ҽ��� ���� �������� ������.
		result = pVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		if (result < 0)
			return;
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		pVertexBuffer->Unmap(0, nullptr);

		//���� ���� �並 �ʱ�ȭ
		vertexBufferView.BufferLocation = pVertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(Vertex);
		vertexBufferView.SizeInBytes = sizeof(triangleVertices);
	}


	/// <summary>
	///�ε��� ���� �����
	/// </summary>
	{
		uint32_t indices[] = { 0, 1, 2 ,  0,3,1 };

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = sizeof(indices);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		result = pD3d12Device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(*pIndexBuffer), (void**)&pIndexBuffer);
		if (result < 0)
			return;

		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);
		result = pIndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		if (result < 0)
			return;
		memcpy(pVertexDataBegin, indices, sizeof(indices));
		pIndexBuffer->Unmap(0, nullptr);

		indexBufferView.BufferLocation = pIndexBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		indexBufferView.SizeInBytes = sizeof(indices);
	}
	

	/// <summary>
	/// �ؽ��� �����
	/// </summary>
	ID3D12Resource* pTextureUploadHeap;

	{
		//TexMetadata metadata = {};
		//ScratchImage scratchImg = {};
		//result = LoadFromWICFile(L"../img/background.png", WIC_FLAGS_NONE, &metadata, scratchImg);
		//auto img = scratchImg.GetImage(0, 0, 0);


		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_CUSTOM;  //D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;//D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;//D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 0;//1;
		prop.VisibleNodeMask = 0;// 1;


		D3D12_RESOURCE_DESC desc = {};
		//desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; //D3D12_RESOURCE_DIMENSION_BUFFER;
		//desc.Alignment = 0;
		//desc.Width = TextureWidth;
		//desc.Height = TextureHeight;
		//desc.DepthOrArraySize = 1;
		//desc.MipLevels = 1;
		//desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//DXGI_FORMAT_UNKNOWN;
		//desc.SampleDesc.Count = 1;
		//desc.SampleDesc.Quality = 0;
		////desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		//desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		//desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
		//desc.Alignment = 0;
		//desc.Width = static_cast<UINT>(metadata.width);
		//desc.Height = static_cast<UINT>(metadata.height);
		//desc.DepthOrArraySize = static_cast<uint16_t>(metadata.arraySize);
		//desc.MipLevels = static_cast<uint16_t>(metadata.mipLevels);
		//desc.Format = metadata.format;//DXGI_FORMAT_UNKNOWN;
		//desc.SampleDesc.Count = 1;
		//desc.SampleDesc.Quality = 0;
		//desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		//desc.Flags = D3D12_RESOURCE_FLAG_NONE;


		int imageBytesPerRow;
		int imageSize = LoadImageDataFromFile(&imageData, desc, L"../img/background.png", imageBytesPerRow);

		result = pD3d12Device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			__uuidof(*pTexture), (void**)&pTexture);
		if (result < 0)
			return;


		/*result = pTexture->WriteToSubresource(0,
			nullptr,
			img->pixels,
			static_cast<UINT>(img->rowPitch),
			static_cast<UINT>(img->slicePitch)
		);*/

		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = GetRequiredIntermediateSize(pTexture, 0, 1);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		result = pD3d12Device->CreateCommittedResource(
			//&keep(CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)),
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			//&keep(CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(pTexture, 0, 1))),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			__uuidof(*pTextureUploadHeap), (void**)&pTextureUploadHeap);
		if (result < 0)
			return;

		//�߰� ���ε� ���� �����͸� ������ ���� ���縦 ����
		//���ε� ������ Texture2D��
		//std::vector<UINT8> texture = GenerateTextureData();


		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = &imageData[0];
		textureData.RowPitch = imageBytesPerRow;  //TextureWidth * TexturePixelSize;
		textureData.SlicePitch = imageBytesPerRow * desc.Height; //textureData.RowPitch * TextureHeight;
		UpdateSubresources(pCommandList, pTexture, pTextureUploadHeap, 0, 0, 1, &textureData);

		D3D12_RESOURCE_BARRIER BarrierDesc = {};
		BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		BarrierDesc.Transition.pResource = pTexture;
		BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		pCommandList->ResourceBarrier(1, &BarrierDesc);


		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		pD3d12Device->CreateShaderResourceView(pTexture, &srvDesc, pSrvHeap->GetCPUDescriptorHandleForHeapStart());

	}
	//	��� ����� �ݰ� �����Ͽ� �ʱ� GPU ������ �����մϴ�.
	result = pCommandList->Close();
	if (result < 0)
		return;

	ID3D12CommandList* ppCommandLists[] ={ pCommandList };
	pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);


	pTextureUploadHeap->Release();
	pTextureUploadHeap = nullptr;

	/// <summary>
	/// ��Ÿ�� ��ü �����
	/// </summary>
	{
		result = pD3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(*pFence), (void**)&pFence);
		if (result < 0)
			return;

		fenceValue = 1;

		// ������ ����ȭ�� ����� �̺�Ʈ �ڵ��� �����
		hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (hFenceEvent == nullptr)
		{
			result = HRESULT_FROM_WIN32(GetLastError());
			if (result < 0)
				return;
		}

		// Ŀ��� ����Ʈ�� �Ϸ�ɋ����� ��ٸ�
		WaitForPreviousFrame();
	}

}

void DirectX12Pipline::PopulateCommandList()
{
	// Ŀ��� ����Ʈ�� �Ҵ��ڴ� ����� ��쿡�� �缳��
	// Ŀ��� ����Ʈ�� GPU���� �������� ������ ���� ����
	// GPU ���� ���� ��Ȳ�� �����ϴ� �潺
	result = pCommandAllocator->Reset();
	if (result < 0)
		return;

	// Ư�� Ŀ��� ����Ʈ���� ExecuteCommandList()�� ȣ��Ǵ� ���
	// �ش� ��� ����� �������� �缳���� �� ������ ������ �缳���Ǿ�� ��.
	// �ٽ� �÷����� 
	result = pCommandList->Reset(pCommandAllocator, pPipelineState);
	if (result < 0)
		return;

	// ���¸� ����
	pCommandList->SetGraphicsRootSignature(pRootSignature); //��Ʈ �ñ״��� ����

	ID3D12DescriptorHeap* ppHeaps[] = { pSrvHeap };
	pCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	pCommandList->SetGraphicsRootDescriptorTable(0, pSrvHeap->GetGPUDescriptorHandleForHeapStart());

	pCommandList->RSSetViewports(1, &viewport);			//����Ʈ 
	pCommandList->RSSetScissorRects(1, &scissorRect);     //Ŭ���� �簢��


	//���� ����� �ε��� ��������
	frameIndex = pSwapChain->GetCurrentBackBufferIndex();

	// ���۰� ������ ������� ���� 
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = pRenderTargets[frameIndex];
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	pCommandList->ResourceBarrier(1, &BarrierDesc);
	//pCommandList->ResourceBarrier(1, &keep(CD3DX12_RESOURCE_BARRIER::Transition(pRenderTargets[frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET)));
	pCommandList->SetPipelineState(pPipelineState);
	//���� Ÿ�� ����
	//CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pRtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize); 
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = pRtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += static_cast<ULONG_PTR>(frameIndex * pD3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	pCommandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	// Ŀ��� ���
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //Ʈ���̿��� ����Ʈ�θ����
	pCommandList->IASetVertexBuffers(0, 1, &vertexBufferView); //���ؽ����ۺ�� ũ���
	pCommandList->IASetIndexBuffer(&indexBufferView); //�ε��� ����
	//pCommandList->DrawInstanced(6, 1, 0, 0);//�� 3�� �ν��Ͻ�1��
	pCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

	// ���� ����۰� ȭ������ ����
	//pCommandList->ResourceBarrier(1, &keep(CD3DX12_RESOURCE_BARRIER::Transition(pRenderTargets[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT)));
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	pCommandList->ResourceBarrier(1, &BarrierDesc);

	result = pCommandList->Close();
	if (result < 0)
		return;
}

void DirectX12Pipline::WaitForPreviousFrame()
{
	// ����ϱ� ���� �������� �Ϸ�� ������ ��ٸ��� ���� �ּ��� ����� �ƴ�
	// �ܼ�ȭ�� ���� �̷��� ������ �ڵ�
	// ������ ȿ������ ���ҽ� ����� ���� �潺�� ����ϴ� ����� ������
	// GPU Ȱ�뵵�� �ִ�ȭ�մϴ�.

	// ��ȣ�� ������ �潺�� ����
	const UINT64 fence = fenceValue;
	result = pCommandQueue->Signal(pFence, fence);
	if (result < 0)
		return;

	fenceValue++;

	// ���� �������� �Ϸ�ɶ����� ��ٸ�
	if (pFence->GetCompletedValue() < fence)
	{
		result = pFence->SetEventOnCompletion(fence, hFenceEvent);
		if (result < 0)
			return;
		WaitForSingleObject(hFenceEvent, INFINITE);
	}

	frameIndex = pSwapChain->GetCurrentBackBufferIndex();
}

std::vector<UINT8> DirectX12Pipline::GenerateTextureData()
{
	const UINT rowPitch = TextureWidth * TexturePixelSize;
	const UINT cellPitch = rowPitch >> 3;        // üũ���� �ؽ�ó�� �� �ʺ��Դϴ�.
	const UINT cellHeight = TextureWidth >> 3;    // üĿ���� �ؽ�ó�� �� �����Դϴ�.
	const UINT textureSize = rowPitch * TextureHeight;

	std::vector<UINT8> data(textureSize);
	UINT8* pData = &data[0];

	for (UINT n = 0; n < textureSize; n += TexturePixelSize)
	{
		UINT x = n % rowPitch;
		UINT y = n / rowPitch;
		UINT i = x / cellPitch;
		UINT j = y / cellHeight;

		if (i % 2 == j % 2)
		{
			pData[n] = 0x00;        // R
			pData[n + 1] = 0x00;    // G
			pData[n + 2] = 0x00;    // B
			pData[n + 3] = 0xff;    // A
		}
		else
		{
			pData[n] = 0xff;        // R
			pData[n + 1] = 0xff;    // G
			pData[n + 2] = 0xff;    // B
			pData[n + 3] = 0xff;    // A
		}
	}

	return data;
}


//�ϵ���� ���(�׷���ī��)ã�Ƴ���
void DirectX12Pipline::GetHardwareAdapter(_In_ IDXGIFactory1* pFactory, _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter)
{
	*ppAdapter = nullptr; //�ʱⰪ
	IDXGIAdapter1* pAapter = nullptr;
	IDXGIFactory6* pFactory6 = nullptr;
	int result = 0;

	result = pFactory->QueryInterface(__uuidof(*pFactory6), (void**)&pFactory6);

	if (result >= 0)
	{
		for (
			UINT adapterIndex = 0;
			SUCCEEDED(pFactory6->EnumAdapterByGpuPreference(
				adapterIndex,
				requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
				__uuidof(*pAapter), (void**)&pAapter));
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc; // Ȥ�� ����ü���� �ʿ��� �� �������� �𸣴�
			pAapter->GetDesc1(&desc);



			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// �⺻ ������ ����̹��� �����ϸ� �ȵ�
				// ����Ʈ���� ��Ͱ� �ʿ��ϸ� /warp�� ������
				continue;
			}

			//�ڲ� ���� �׷���ī��� ��Ƽ� ¥������ �����Ʒ� ��� �ڵ��ۼ�
			std::wstring strDesc = desc.Description;
			if (strDesc.find(L"NVIDIA") != std::string::npos) {
				//*ppAdapter = pAapter;
				break;
			}
			//// �ϵ���� �����ϴ� ��� �����Ѵٸ� �����Ѵ�
			//if (SUCCEEDED(D3D12CreateDevice(pAapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			//{
			//    break;
			//}
		}
	}

	if (pAapter == nullptr) // Ȥ�� ��Ͱ� �������� �ʴ´ٸ�
	{
		for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &pAapter)); ++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			pAapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{

				continue;
			}

			// ��ġ�� 12�������ϴ� Ȯ�������� �������� �ʴ´�
			if (SUCCEEDED(D3D12CreateDevice(pAapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
			{
				break;
			}
		}
	}

	*ppAdapter = pAapter;

	//if (pAapter != nullptr) {
	//    pAapter->Release();
	//    pAapter = nullptr;
	//}
	if (pFactory6 != nullptr) {
		pFactory6->Release();
		pFactory6 = nullptr;
	}
}

void DirectX12Pipline::GetAssetFullPath(LPCWSTR assetName, wchar_t* result)
{
	result[0] = L'\0';
	wcscpy_s(result, 512, assetsPath);
	wcscat_s(result, 512, assetName);
}


void DirectX12Pipline::GetAssetsPath(WCHAR* path, UINT pathSize)
{
	//GetCurrentDirectory �̰� ���� �׷���?
	if (path == nullptr)
	{
		throw std::exception();
	}

	DWORD size = GetModuleFileName(nullptr, path, pathSize);
	if (size == 0 || size == pathSize)
	{
		throw std::exception();
	}

	WCHAR* lastSlash = wcsrchr(path, L'\\');
	if (lastSlash)
	{
		*(lastSlash + 1) = L'\0';
	}
}

int DirectX12Pipline::LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename, int& bytesPerRow)
{
	int hr;
	IWICImagingFactory* wicFactory=nullptr; 	//���ڴ��� �������� �����Ϸ��� �ν��Ͻ� �ʿ�
	
	//�ε��ϴ� �̹������� �ٸ��� ������ �缳��
	IWICBitmapDecoder* wicDecoder = nullptr;
	IWICBitmapFrameDecode* wicFrame = nullptr;
	IWICFormatConverter* wicConverter = nullptr;

	bool imageConverted = false;

	if (wicFactory == nullptr)
	{
		CoInitialize(nullptr);  //�ʱ�ȭ

		// WIC ���丮 ����
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			__uuidof(*wicFactory), (void**)&wicFactory);
		if (hr < 0)
			return 0;

		hr = wicFactory->CreateFormatConverter(&wicConverter);
		if (hr < 0)
			return 0;
	}

	//�̹��� ���ڴ� �ε�
	hr = wicFactory->CreateDecoderFromFilename(
		filename,							//�̹��� ���� �̸�
		nullptr,                            //���޾�ü ID (�̰� ���̳ʸ� �ش����Ͽ� �ִµ� ������ �Ⱦ��� ������ nullptr ����)
		GENERIC_READ,						// ��� ����� ������
		WICDecodeMetadataCacheOnLoad,		// ������ �ð��� ����ϴ� �ͺ��� ���� ��� ��Ÿ�����͸� ĳ��
		&wicDecoder							// ������ WIC ���ڴ�
	);
	if (hr < 0)
		return 0;

	// ���ڴ� �̹��� �������� (�������� ���ڵ���)
	hr = wicDecoder->GetFrame(0, &wicFrame);
	if (hr < 0)
		return 0;

	// �̹����� WIC �ȼ� ������ ������
	WICPixelFormatGUID pixelFormat;
	hr = wicFrame->GetPixelFormat(&pixelFormat);
	if (hr < 0)
		return 0;

	// �̹����� ũ�⸦ ������
	UINT textureWidth, textureHeight;
	hr = wicFrame->GetSize(&textureWidth, &textureHeight);
	if (hr < 0)
		return 0;

	//sRGB ������ ���ؼ� ó������ �������� �ʿ��ϸ� ���� �ٽ� �����ߵ�

	// WIC �ȼ� ������ dxgi �ȼ��������� ��ȯ
	DXGI_FORMAT dxgiFormat = GetDXGIFormatFromWICFormat(pixelFormat);

	// �̹��� ������ �����Ǵ� dixg�����̾ƴϸ� ��ȯ �õ�
	if (dxgiFormat == DXGI_FORMAT_UNKNOWN)
	{
		// ���� �̹��� ���Ŀ��� dxgi ȣȯ WIC ������ ������
		WICPixelFormatGUID convertToPixelFormat = GetConvertToWICFormat(pixelFormat);
		if (convertToPixelFormat == GUID_WICPixelFormatDontCare) // dxgi ȣȯ ������ �߰ߵ��� ���� ��� ��ȯ
			return 0;

		dxgiFormat = GetDXGIFormatFromWICFormat(convertToPixelFormat);	// dxgi ���� ����

		// dxgi ȣȯ �������� ��ȯ�Ҽ� �ִ��� Ȯ��
		BOOL canConvert = FALSE;
		hr = wicConverter->CanConvert(pixelFormat, convertToPixelFormat, &canConvert);
		if (hr < 0 || !canConvert) 
			return 0;

		// ��ȯ�� �����մϴ�(wicConverter���� ��ȯ�� �̹����� ����)
		hr = wicConverter->Initialize(wicFrame, convertToPixelFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
		if (hr < 0)
			return 0;

		// wicConverter���� �̹��� �����͸� �������� ����� �˱� ���ؼ� �÷��� ��ȯ
		// �ƴϸ� wicFrame���� �����;ߵ�
		imageConverted = true;
	}

	int bitsPerPixel = GetDXGIFormatBitsPerPixel(dxgiFormat); // �ȼ��� ��Ʈ��
	bytesPerRow = (textureWidth * bitsPerPixel) / 8; //�̹��� �������� �� �࿡ �ִ� ����Ʈ ��
	int imageSize = bytesPerRow * textureHeight; // �� �̹��� ũ��

	//���� �̹��� �����Ϳ� ����� �޸𸮸� �Ҵ��ϰ� �ش� �޸𸮸� ����Ű���� imageData�� ����
	*imageData = new BYTE[imageSize];

	// 	���� �Ҵ�� �޸�(imageData)�� ���� �̹��� �����͸� ����(���ڵ�)
	if (imageConverted)
	{
		//�̹��� ������ ��ȯ�ؾ� �ϴ� ��� wic ��ȯ�⿡�� ��ȯ�� �̹����� ����
		hr = wicConverter->CopyPixels(0, bytesPerRow, imageSize, *imageData);
		if (hr < 0)
			return 0;
	}
	else
	{
		//��ȯ�� �ʿ䰡 �����ϴ�.wic �����ӿ��� �����͸� �����ϱ⸸ �ϸ��
		hr = wicFrame->CopyPixels(0, bytesPerRow, imageSize, *imageData);
		if (hr < 0)
			return 0;
	}

	resourceDescription = {};
	resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDescription.Alignment = 0; // 0, 4KB, 64KB �Ǵ� 4MB�� �� �ֽ��ϴ�. 0�� ��Ÿ���� 64KB�� 4MB(��Ƽ ���ø� �ؽ�ó�� ��� 4MB) ���̿��� ����
	resourceDescription.Width = textureWidth; 
	resourceDescription.Height = textureHeight; 
	resourceDescription.DepthOrArraySize = 1; //3D �̹����� ��� 3D �̹����� �����Դϴ�.�׷��� ������ 1D �Ǵ� 2D �ؽ�ó �迭(�̹����� �ϳ����̹Ƿ� 1�� ����)
	resourceDescription.MipLevels = 1; //�Ӹ� ��. �� �ؽ�ó�� ���� �Ӹ��� �������� �����Ƿ� ������ �ϳ��� ����
	resourceDescription.Format = dxgiFormat; //�̰��� �̹����� dxgi �����Դϴ�(�ȼ� ����).
	resourceDescription.SampleDesc.Count = 1; // �̰��� �ȼ��� ���� ���Դϴ�. �츮�� ���� 1���� ����
	resourceDescription.SampleDesc.Quality = 0; // ������ ǰ�� �����Դϴ�. �������� ǰ���� ���������� ���ɶ�����
	resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; //�ȼ��� �迭�Դϴ�.�� �� �������� �����ϸ� �˾Ƽ� ���� ȿ������ ���� ��������
	resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE; // �÷��� ����

	// �̹����� ũ�⸦ ��ȯ�մϴ�. �۾��� ������ �̹����� �����ϴ� ���� ���� ������
	wicDecoder->Release();
	wicDecoder = nullptr;

	wicFrame->Release();
	wicFrame = nullptr;

	wicConverter->Release();
	wicConverter = nullptr;

	return imageSize;
}

//wic ���İ� ������ dxgi ����
DXGI_FORMAT DirectX12Pipline::GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
	if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFloat) 
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAHalf) 
		return DXGI_FORMAT_R16G16B16A16_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBA) 
		return DXGI_FORMAT_R16G16B16A16_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA) 
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGRA) 
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR) 
		return DXGI_FORMAT_B8G8R8X8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102XR) 
		return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102)
		return DXGI_FORMAT_R10G10B10A2_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGRA5551)
		return DXGI_FORMAT_B5G5R5A1_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR565)
		return DXGI_FORMAT_B5G6R5_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFloat)
		return DXGI_FORMAT_R32_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayHalf) 
		return DXGI_FORMAT_R16_FLOAT;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGray)
		return DXGI_FORMAT_R16_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppGray) 
		return DXGI_FORMAT_R8_UNORM;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppAlpha) 
		return DXGI_FORMAT_A8_UNORM;

	else return DXGI_FORMAT_UNKNOWN;
}

//�ٸ� WIC ���Ŀ��� dxgi ȣȯ WIC ������ ������.
WICPixelFormatGUID DirectX12Pipline::GetConvertToWICFormat(WICPixelFormatGUID& wicFormatGUID)
{
	if (wicFormatGUID == GUID_WICPixelFormatBlackWhite) 
		return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat1bppIndexed) 
		return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat2bppIndexed) 
		return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat4bppIndexed) 
		return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat8bppIndexed) 
		return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat2bppGray) 
		return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat4bppGray) 
		return GUID_WICPixelFormat8bppGray;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayFixedPoint) 
		return GUID_WICPixelFormat16bppGrayHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFixedPoint)
		return GUID_WICPixelFormat32bppGrayFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR555)
		return GUID_WICPixelFormat16bppBGRA5551;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR101010)
		return GUID_WICPixelFormat32bppRGBA1010102;
	else if (wicFormatGUID == GUID_WICPixelFormat24bppBGR) 
		return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat24bppRGB) 
		return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppPBGRA) 
		return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppPRGBA)
		return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGB) 
		return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppBGR) 
		return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRA) 
		return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBA)
		return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPBGRA) 
		return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBFixedPoint) 
		return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppBGRFixedPoint) 
		return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAFixedPoint) 
		return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppBGRAFixedPoint)
		return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBFixedPoint) 
		return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBHalf) 
		return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat48bppRGBHalf) 
		return GUID_WICPixelFormat64bppRGBAHalf;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppPRGBAFloat)
		return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFloat)
		return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFixedPoint)
		return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat128bppRGBFixedPoint)
		return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBE) 
		return GUID_WICPixelFormat128bppRGBAFloat;
	else if (wicFormatGUID == GUID_WICPixelFormat32bppCMYK) 
		return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppCMYK)
		return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat40bppCMYKAlpha) 
		return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat80bppCMYKAlpha)
		return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
	else if (wicFormatGUID == GUID_WICPixelFormat32bppRGB) 
		return GUID_WICPixelFormat32bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppRGB) 
		return GUID_WICPixelFormat64bppRGBA;
	else if (wicFormatGUID == GUID_WICPixelFormat64bppPRGBAHalf) 
		return GUID_WICPixelFormat64bppRGBAHalf;
#endif

	else return GUID_WICPixelFormatDontCare;
}

int DirectX12Pipline::GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat)
{
	if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT) 
		return 128;
	else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) 
		return 64;
	else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) 
		return 64;
	else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM) 
		return 32;
	else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM) 
		return 32;
	else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM) 
		return 32;
	else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM)
		return 32;

	else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) 
		return 32;
	else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM) 
		return 16;
	else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM)
		return 16;
	else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT)
		return 32;
	else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT) 
		return 16;
	else if (dxgiFormat == DXGI_FORMAT_R16_UNORM)
		return 16;
	else if (dxgiFormat == DXGI_FORMAT_R8_UNORM) 
		return 8;
	else if (dxgiFormat == DXGI_FORMAT_A8_UNORM) 
		return 8;
}
