#include "stdafx.h"
#include "DirectX12Pipline.h"
#include "WinAPI.h"


DirectX12Pipline::DirectX12Pipline() : pD3d12Device(nullptr), pCommandQueue(nullptr), pSwapChain(nullptr), pRtvHeap(nullptr), pRenderTargets{nullptr}, pCommandAllocator(nullptr), pCommandList(nullptr), pFence(nullptr), hFenceEvent(nullptr), pPipelineState(nullptr),result(0), frameIndex(0), rtvDescriptorSize(0), fenceValue(0)
{
}

DirectX12Pipline::~DirectX12Pipline()
{
}

void DirectX12Pipline::OnInit()
{
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
    result =pSwapChain->Present(1, 0);
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


// ������ ���������� �ε�
void DirectX12Pipline::LoadPipeline()
{
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // ����� ���̾ Ȱ��ȭ�մϴ�(�׷��� ������ "������ ���" �ʿ�).
    // ����: ��ġ ���� �� ����� ������ Ȱ��ȭ�ϸ� Ȱ�� ��ġ�� ��ȿȭ�˴ϴ�.
    {
        ID3D12Debug* pDebugController = nullptr;
        result = D3D12GetDebugInterface(__uuidof(*pDebugController), (void**)&pDebugController);
        if (result)
        {
            pDebugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG; // �߰� ����� ���̾ Ȱ��ȭ�մϴ�.
        }
        pDebugController->Release();
    }
#endif
    IDXGIFactory4* pFactory = nullptr;
     result = CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(*pFactory), (void**)&pFactory);
    if (result < 0)
        return;


    if (useWarpDevice)
    {
        IDXGIAdapter *pWarpAdapter=nullptr;

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

        GetHardwareAdapter(pFactory, &pHardwareAdapter);
        result = D3D12CreateDevice(pHardwareAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(*pD3d12Device), (void**)&pD3d12Device);
        if (result < 0)
            return;

        pHardwareAdapter->Release();
        pHardwareAdapter = nullptr;
    }

    // Ŀ��� ť�� �����ϰ� ����
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
 
    result = pD3d12Device->CreateCommandQueue(&queueDesc, __uuidof(*pCommandQueue), (void**)&pCommandQueue);
    if (result < 0)
        return;

    // ����ü��(�����)�� �����ϰ� ����
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = WinAPI::pWinAPI->GetWidth();
    swapChainDesc.Height = WinAPI::pWinAPI->GetHeigth();
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    IDXGISwapChain1 *tempSwapChain;
    result =pFactory->CreateSwapChainForHwnd(
        pCommandQueue,        // ����ü���� ����� �÷��ø� �ϱ����ؼ��� ��⿭�� �ʿ�
        WinAPI::pWinAPI->GetHwnd(),
        &swapChainDesc,
        nullptr,
        nullptr,
        &tempSwapChain
    );
    if (result < 0)
        return;
    
    
    result = pFactory->MakeWindowAssociation(WinAPI::pWinAPI->GetHwnd(), DXGI_MWA_NO_ALT_ENTER);
    if (result < 0)
        return;
    //tempSwapChain.As(&pSwapChain);
    result = tempSwapChain->QueryInterface(__uuidof(pSwapChain), (void**)&pSwapChain);
    if (result < 0)
        return;


    frameIndex = pSwapChain->GetCurrentBackBufferIndex();

    // ���� ����
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
            result =pSwapChain->GetBuffer(i, __uuidof(*pRenderTargets[i]), (void**)&pRenderTargets[i]);
            pD3d12Device->CreateRenderTargetView(pRenderTargets[i], nullptr, rtvHandle);
            rtvHandle.Offset(1, rtvDescriptorSize);
        }
    }
    result=pD3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(*pCommandAllocator), (void**)&pCommandAllocator);
    if (result < 0)
        return;

    tempSwapChain->Release();
    tempSwapChain = nullptr;
    pFactory->Release();
    pFactory = nullptr;
}

//������ �ε�
void DirectX12Pipline::LoadAssets()
{
    //Ŀ��� ����Ʈ�� �����.
    result = pD3d12Device->CreateCommandList(0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        pCommandAllocator,
        nullptr,
        __uuidof(*pCommandList), (void**)&pCommandList);
    if (result < 0)
        return;
    // ��ɾ� ����� �����Ǵµ� �ƹ��͵� ����
    // ���η����� ���������� ���������� ���� ����
    result= pCommandList->Close();
    if (result < 0)
        return;

    // ����ȭ ��ü �����
    {
        result=pD3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(*pFence), (void**)&pFence);
        if (result < 0)
            return;

        fenceValue = 1;

        // ������ ����ȭ�� ����� �̺�Ʈ �ڵ��� �����
        hFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (hFenceEvent == nullptr)
        {
            result=HRESULT_FROM_WIN32(GetLastError());
            if (result < 0)
                return;
        }
    }
}

void DirectX12Pipline::PopulateCommandList()
{
    // Ŀ��� ����Ʈ�� �Ҵ��ڴ� ����� ��쿡�� �缳��
    // Ŀ��� ����Ʈ�� GPU���� �������� ������ ���� ����
    // GPU ���� ���� ��Ȳ�� �����ϴ� �潺
    result= pCommandAllocator->Reset();
    if (result < 0)
        return;

    // Ư�� Ŀ��� ����Ʈ���� ExecuteCommandList()�� ȣ��Ǵ� ���
    // �ش� ��� ����� �������� �缳���� �� ������ ������ �缳���Ǿ�� ��.
    // �ٽ� �÷����� 
    result =pCommandList->Reset(pCommandAllocator, pPipelineState);
    if (result < 0)
        return;

    // ���۰� ������ ������� ���� 
    pCommandList->ResourceBarrier(1, &keep(CD3DX12_RESOURCE_BARRIER::Transition(pRenderTargets[frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET)));
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(pRtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);

    // Ŀ��� ���
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // ���� ����۰� ȭ������ ����
    pCommandList->ResourceBarrier(1, &keep(CD3DX12_RESOURCE_BARRIER::Transition(pRenderTargets[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT)));
    ThrowIfFailed(pCommandList->Close());
}

void DirectX12Pipline::WaitForPreviousFrame()
{
    // ����ϱ� ���� �������� �Ϸ�� ������ ��ٸ��� ���� �ּ��� ����� �ƴ�
    // �ܼ�ȭ�� ���� �̷��� ������ �ڵ�
    // ������ ȿ������ ���ҽ� ����� ���� �潺�� ����ϴ� ����� ������
    // GPU Ȱ�뵵�� �ִ�ȭ�մϴ�.

    // ��ȣ�� ������ �潺�� ����
    const UINT64 fence = fenceValue;
    ThrowIfFailed(pCommandQueue->Signal(pFence, fence));
    fenceValue++;

    // ���� �������� �Ϸ�ɶ����� ��ٸ�
    if (pFence->GetCompletedValue() < fence)
    {
        ThrowIfFailed(pFence->SetEventOnCompletion(fence, hFenceEvent));
        WaitForSingleObject(hFenceEvent, INFINITE);
    }

    frameIndex = pSwapChain->GetCurrentBackBufferIndex();
}

void DirectX12Pipline::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter)
{
    *ppAdapter = nullptr; //�ʱⰪ

    IDXGIAdapter1* pAapter=nullptr;
    IDXGIFactory6* pFactory6=nullptr;
    int result = 0;

    result = pFactory->QueryInterface(__uuidof(*pFactory6), (void**)&pFactory6);

    if (result>=0)
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

            // ��ġ�� 12�������ϴ� Ȯ�������� �������� �ʴ´�
            if (SUCCEEDED(D3D12CreateDevice(pAapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
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
