#include "stdafx.h"
#include "DirectX12Pipline.h"
#include "WinAPI.h"


DirectX12Pipline::DirectX12Pipline() : pD3d12Device(nullptr), pCommandQueue(nullptr), pSwapChain(nullptr), pRtvHeap(nullptr), pRenderTargets{ nullptr }, pCommandAllocator(nullptr), pCommandList(nullptr), pFence(nullptr), hFenceEvent(nullptr), pPipelineState(nullptr), 
pRootSignature(nullptr), pVertexBuffer(nullptr), //pIndexBuffer(nullptr),
//viewport(0.0f, 0.0f, 0.0f, 0.0f),
//scissorRect(0, 0, 0, 0),
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

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	scissorRect = { 0, 0, (int)width, (int)height };

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

	//if (pIndexBuffer != nullptr)
	//{
	//	pIndexBuffer->Release();
	//	pIndexBuffer = nullptr;
	//}

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
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.SampleDesc.Count = 1;

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
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		ID3DBlob* pSignature = nullptr;
		ID3DBlob* pError = nullptr;

		result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, &pError);
		if (result < 0)
			return;

		result = pD3d12Device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), __uuidof(*pRootSignature), (void**)&pRootSignature);
		if (result < 0)
			return;

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
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		//�׷��� ���� ����������(PSO)���� �� ����
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = pRootSignature;
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader);
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader);
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.SampleDesc.Count = 1;
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
		//Ŀ��� ����Ʈ�� �� �̻� �� ���� ������ �ݾ� ���´�. 
		result = pCommandList->Close();
		if (result < 0)
			return;
	}


	Model.LoadModel(L"cube.txt");



	/// <summary>
	/// ���ý� ���� �����
	/// </summary>
	{
		//Vertex triangleVertices[] =
		//{
		//	{ { -0.25f, 0.25f * aspectRatio, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		//	{ { 0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		//	{ { -0.25f, -0.25f * aspectRatio, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
		//	//{ { 0.25f, 0.25f * aspectRatio, 0.0f }, { 0.0f, 0.5f, 0.5f, 1.0f } }
		//};

		D3D12_HEAP_PROPERTIES prop = {};
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		prop.CreationNodeMask = 1;
		prop.VisibleNodeMask = 1;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = Model.GetModelSize();
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		//const UINT vertexBufferSize = sizeof(triangleVertices);
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
		result =pVertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		if (result < 0)
			return;
		memcpy(pVertexDataBegin, Model.GetModel(), Model.GetModelSize());
		pVertexBuffer->Unmap(0, nullptr);

		//���� ���� �並 �ʱ�ȭ
		vertexBufferView.BufferLocation = pVertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(Vertex);
		vertexBufferView.SizeInBytes = Model.GetModelSize();
	}

	////�ε��� ���� �����
	//{
	//	uint32_t indices[] = { 0, 1, 2 ,  0,3,1 };

	//	D3D12_HEAP_PROPERTIES prop = {};
	//	prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	//	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	//	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	//	prop.CreationNodeMask = 1;
	//	prop.VisibleNodeMask = 1;

	//	D3D12_RESOURCE_DESC desc = {};
	//	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//	desc.Alignment = 0;
	//	desc.Width = sizeof(indices);
	//	desc.Height = 1;
	//	desc.DepthOrArraySize = 1;
	//	desc.MipLevels = 1;
	//	desc.Format = DXGI_FORMAT_UNKNOWN;
	//	desc.SampleDesc.Count = 1;
	//	desc.SampleDesc.Quality = 0;
	//	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	//	result = pD3d12Device->CreateCommittedResource(
	//		&prop,
	//		D3D12_HEAP_FLAG_NONE,
	//		&desc,
	//		D3D12_RESOURCE_STATE_GENERIC_READ,
	//		nullptr, 
	//		__uuidof(*pIndexBuffer), (void**)&pIndexBuffer);
	//	if (result < 0)
	//		return;

	//	UINT8* pVertexDataBegin;
	//	CD3DX12_RANGE readRange(0, 0);
	//	result = pIndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	//	if (result < 0)
	//		return;
	//	memcpy(pVertexDataBegin, indices, sizeof(indices));
	//	pIndexBuffer->Unmap(0, nullptr);

	//	indexBufferView.BufferLocation = pIndexBuffer->GetGPUVirtualAddress();
	//	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	//	indexBufferView.SizeInBytes = sizeof(indices);
	//}				  


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
	//pCommandList->IASetIndexBuffer(&indexBufferView); //�ε��� ����
	pCommandList->DrawInstanced(Model.GetModelFaceCount(), 1, 0, 0);
	//pCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

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
