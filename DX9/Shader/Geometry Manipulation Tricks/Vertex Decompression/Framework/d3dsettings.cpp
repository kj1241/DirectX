#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include <D3D9.h>
#include "DXUtil.h"
#include "D3DUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "resource.h"

CD3DSettingsDialog* s_pSettingsDialog = NULL;

//�־��� D3DDEVTYPE�� ���� ���ڿ��� ��ȯ�մϴ�.
TCHAR* D3DDevTypeToString(D3DDEVTYPE devType)
{
    switch (devType)
    {
    case D3DDEVTYPE_HAL:        return TEXT("D3DDEVTYPE_HAL");
    case D3DDEVTYPE_SW:         return TEXT("D3DDEVTYPE_SW");
    case D3DDEVTYPE_REF:        return TEXT("D3DDEVTYPE_REF");
    default:                    return TEXT("Unknown devType");
    }
}

//�־��� D3DMULTISAMPLE_TYPE�� ���� ���ڿ��� ��ȯ�մϴ�.
TCHAR* MultisampleTypeToString(D3DMULTISAMPLE_TYPE MultiSampleType)
{
    switch (MultiSampleType)
    {
    case D3DMULTISAMPLE_NONE:   return TEXT("D3DMULTISAMPLE_NONE");
    case D3DMULTISAMPLE_NONMASKABLE: return TEXT("D3DMULTISAMPLE_NONMASKABLE");
    case D3DMULTISAMPLE_2_SAMPLES: return TEXT("D3DMULTISAMPLE_2_SAMPLES");
    case D3DMULTISAMPLE_3_SAMPLES: return TEXT("D3DMULTISAMPLE_3_SAMPLES");
    case D3DMULTISAMPLE_4_SAMPLES: return TEXT("D3DMULTISAMPLE_4_SAMPLES");
    case D3DMULTISAMPLE_5_SAMPLES: return TEXT("D3DMULTISAMPLE_5_SAMPLES");
    case D3DMULTISAMPLE_6_SAMPLES: return TEXT("D3DMULTISAMPLE_6_SAMPLES");
    case D3DMULTISAMPLE_7_SAMPLES: return TEXT("D3DMULTISAMPLE_7_SAMPLES");
    case D3DMULTISAMPLE_8_SAMPLES: return TEXT("D3DMULTISAMPLE_8_SAMPLES");
    case D3DMULTISAMPLE_9_SAMPLES: return TEXT("D3DMULTISAMPLE_9_SAMPLES");
    case D3DMULTISAMPLE_10_SAMPLES: return TEXT("D3DMULTISAMPLE_10_SAMPLES");
    case D3DMULTISAMPLE_11_SAMPLES: return TEXT("D3DMULTISAMPLE_11_SAMPLES");
    case D3DMULTISAMPLE_12_SAMPLES: return TEXT("D3DMULTISAMPLE_12_SAMPLES");
    case D3DMULTISAMPLE_13_SAMPLES: return TEXT("D3DMULTISAMPLE_13_SAMPLES");
    case D3DMULTISAMPLE_14_SAMPLES: return TEXT("D3DMULTISAMPLE_14_SAMPLES");
    case D3DMULTISAMPLE_15_SAMPLES: return TEXT("D3DMULTISAMPLE_15_SAMPLES");
    case D3DMULTISAMPLE_16_SAMPLES: return TEXT("D3DMULTISAMPLE_16_SAMPLES");
    default:                    return TEXT("Unknown Multisample Type");
    }
}

//������ VertexProcessingType�� ���� ���ڿ��� ��ȯ�մϴ�.
TCHAR* VertexProcessingTypeToString(VertexProcessingType vpt)
{
    switch (vpt)
    {
    case SOFTWARE_VP:      return TEXT("SOFTWARE_VP");
    case MIXED_VP:         return TEXT("MIXED_VP");
    case HARDWARE_VP:      return TEXT("HARDWARE_VP");
    case PURE_HARDWARE_VP: return TEXT("PURE_HARDWARE_VP");
    default:               return TEXT("Unknown VertexProcessingType");
    }
}

//�־��� ���� ���ݿ� ���� ���ڿ��� ��ȯ�մϴ�.
TCHAR* PresentIntervalToString( UINT pi )
{
    switch( pi )
    {
    case D3DPRESENT_INTERVAL_IMMEDIATE: return TEXT("D3DPRESENT_INTERVAL_IMMEDIATE");
    case D3DPRESENT_INTERVAL_DEFAULT:   return TEXT("D3DPRESENT_INTERVAL_DEFAULT");
    case D3DPRESENT_INTERVAL_ONE:       return TEXT("D3DPRESENT_INTERVAL_ONE");
    case D3DPRESENT_INTERVAL_TWO:       return TEXT("D3DPRESENT_INTERVAL_TWO");
    case D3DPRESENT_INTERVAL_THREE:     return TEXT("D3DPRESENT_INTERVAL_THREE");
    case D3DPRESENT_INTERVAL_FOUR:      return TEXT("D3DPRESENT_INTERVAL_FOUR");
    default:                            return TEXT("Unknown PresentInterval");
    }
}

INT_PTR CALLBACK DialogProcHelper( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    return s_pSettingsDialog->DialogProc( hDlg, msg, wParam, lParam );
}

CD3DSettingsDialog::CD3DSettingsDialog( CD3DEnumeration* pEnumeration, CD3DSettings* pSettings)
{
    s_pSettingsDialog = this;
    m_pEnumeration = pEnumeration;
    m_d3dSettings = *pSettings;
}

//�޺� ���ڿ� �׸��� �߰��մϴ�.
void CD3DSettingsDialog::ComboBoxAdd( int id, void* pData, TCHAR* pstrDesc )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    DWORD dwItem = ComboBox_AddString( hwndCtrl, pstrDesc );
    ComboBox_SetItemData( hwndCtrl, dwItem, pData );
}

//�޺� ���ڿ��� �׸��� �����մϴ�.
void CD3DSettingsDialog::ComboBoxSelect( int id, void* pData )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    UINT count = ComboBoxCount( id );
    for( UINT iItem = 0; iItem < count; iItem++ )
    {
        if( (void*)ComboBox_GetItemData( hwndCtrl, iItem ) == pData )
        {
            ComboBox_SetCurSel( hwndCtrl, iItem );
            PostMessage( m_hDlg, WM_COMMAND, 
                MAKEWPARAM( id, CBN_SELCHANGE ), (LPARAM)hwndCtrl );
            return;
        }
    }
}

//�޺� ���ڿ��� �׸��� �����մϴ�.
void CD3DSettingsDialog::ComboBoxSelectIndex( int id, int index )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    ComboBox_SetCurSel( hwndCtrl, index );
    PostMessage( m_hDlg, WM_COMMAND, MAKEWPARAM( id, CBN_SELCHANGE ), 
        (LPARAM)hwndCtrl );
}

//�޺� ���ڿ��� ������ �׸� ���� �����͸� ��ȯ�մϴ�.
void* CD3DSettingsDialog::ComboBoxSelected( int id )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    int index = ComboBox_GetCurSel( hwndCtrl );
    if( index < 0 )
        return NULL;
    return (void*)ComboBox_GetItemData( hwndCtrl, index );
}

//�̰��� �����ؾ� �� ��, �޺� ������ �׸��� ���õǾ����� ���θ� ��ȯ�մϴ�.  
//ComboBoxSelected()���� �� �����մϴ�.
//������ �׸��� ���� �Ͱ� ������ �׸��� �ִ� �� ���� itemData�� NULL�Դϴ�.
bool CD3DSettingsDialog::ComboBoxSomethingSelected( int id )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    int index = ComboBox_GetCurSel( hwndCtrl );
    return ( index >= 0 );
}

//�޺� ������ �׸� ���� ��ȯ�մϴ�.
UINT CD3DSettingsDialog::ComboBoxCount( int id )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    return ComboBox_GetCount( hwndCtrl );
}

//�޺� ������ �׸��� ����ϴ�.
void CD3DSettingsDialog::ComboBoxClear( int id )
{
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    ComboBox_ResetContent( hwndCtrl );
}

//�޺� ���ڿ� ������ �ؽ�Ʈ�� ���ԵǾ� �ִ��� ���θ� ��ȯ�մϴ�.
bool CD3DSettingsDialog::ComboBoxContainsText( int id, TCHAR* pstrText )
{
    TCHAR strItem[200];
    HWND hwndCtrl = GetDlgItem( m_hDlg, id );
    UINT count = ComboBoxCount( id );
    for( UINT iItem = 0; iItem < count; iItem++ )
    {
        if( ComboBox_GetLBTextLen( hwndCtrl, iItem ) >= 200 )
            continue; // �߻��ؼ��� �� ������, �߻��ϴ� ��� ���۸� ����� ���ʽÿ�.
        ComboBox_GetLBText( hwndCtrl, iItem, strItem );
        if( lstrcmp( strItem, pstrText ) == 0 )
            return true;
    }
    return false;
}

//D3D ���� ��ȭ ���ڸ� ǥ���մϴ�.
INT_PTR CD3DSettingsDialog::ShowDialog( HWND hwndParent )
{
    return DialogBox( NULL, MAKEINTRESOURCE( IDD_SELECTDEVICE ), 
        hwndParent, DialogProcHelper );
}

//��ȭ ���ڿ��� â �޽����� ó���մϴ�.
INT_PTR CD3DSettingsDialog::DialogProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER( lParam );

    switch( msg )
    {
    case WM_INITDIALOG:
        {
            m_hDlg = hDlg;

            // ����� �޺� ���ڸ� ä��ϴ�.  ������ ����͸� ������Ʈ�ϸ� Ʈ���ŵ˴ϴ�.
            // ��ȭ ������ ������ �κ��� ������Ʈ�մϴ�.
            for( UINT iai = 0; iai < m_pEnumeration->m_pAdapterInfoList->Count(); iai++ )
            {
                D3DAdapterInfo* pAdapterInfo;
                pAdapterInfo = (D3DAdapterInfo*)m_pEnumeration->m_pAdapterInfoList->GetPtr(iai);
                TCHAR strDescription[512];
                DXUtil_ConvertAnsiStringToGenericCch( strDescription, pAdapterInfo->AdapterIdentifier.Description, 512 );
                ComboBoxAdd( IDC_ADAPTER_COMBO, pAdapterInfo, strDescription );
                if( pAdapterInfo->AdapterOrdinal == m_d3dSettings.AdapterOrdinal() )
                    ComboBoxSelect( IDC_ADAPTER_COMBO, pAdapterInfo );
            }
            if( !ComboBoxSomethingSelected( IDC_ADAPTER_COMBO ) &&
                ComboBoxCount( IDC_ADAPTER_COMBO ) > 0 )
            {
                ComboBoxSelectIndex( IDC_ADAPTER_COMBO, 0 );
            }
        }
        return TRUE;

    case WM_COMMAND:
        switch( LOWORD(wParam) )
        {
        case IDOK:
            EndDialog( hDlg, IDOK );
            break;
        case IDCANCEL:
            EndDialog( hDlg, IDCANCEL );
            break;
        case IDC_ADAPTER_COMBO:
            if( CBN_SELCHANGE == HIWORD(wParam) )
                AdapterChanged();
            break;
        case IDC_DEVICE_COMBO:
            if( CBN_SELCHANGE == HIWORD(wParam) )
                DeviceChanged();
            break;
        case IDC_ADAPTERFORMAT_COMBO:
            if( CBN_SELCHANGE == HIWORD(wParam) )
                AdapterFormatChanged();
            break;
        case IDC_RESOLUTION_COMBO:
            if( CBN_SELCHANGE == HIWORD(wParam) )
                ResolutionChanged();
            break;
        case IDC_REFRESHRATE_COMBO:
            if( CBN_SELCHANGE == HIWORD(wParam) )
                RefreshRateChanged();
            break;
        case IDC_BACKBUFFERFORMAT_COMBO:
            if( CBN_SELCHANGE == HIWORD(wParam) )
                BackBufferFormatChanged();
            break;
        case IDC_DEPTHSTENCILBUFFERFORMAT_COMBO:
            if( CBN_SELCHANGE == HIWORD(wParam) )
                DepthStencilBufferFormatChanged();
            break;
        case IDC_MULTISAMPLE_COMBO:
            if( CBN_SELCHANGE == HIWORD(wParam) )
                MultisampleTypeChanged();
            break;
        case IDC_MULTISAMPLE_QUALITY_COMBO:
            if( CBN_SELCHANGE == HIWORD(wParam) )
                MultisampleQualityChanged();
            break;
        case IDC_VERTEXPROCESSING_COMBO:
            if( CBN_SELCHANGE == HIWORD(wParam) )
                VertexProcessingChanged();
            break;
        case IDC_PRESENTINTERVAL_COMBO:
            if( CBN_SELCHANGE == HIWORD(wParam) )
                PresentIntervalChanged();
            break;
        case IDC_WINDOW:
        case IDC_FULLSCREEN:
            WindowedFullscreenChanged();
            break;
        }
        return TRUE;

    default:
        return FALSE;
    }
}

//������ ������� ���濡 �����մϴ�.
void CD3DSettingsDialog::AdapterChanged( void )
{
    D3DAdapterInfo* pAdapterInfo = (D3DAdapterInfo*)ComboBoxSelected( IDC_ADAPTER_COMBO );
    if( pAdapterInfo == NULL )
        return;
    
    if( m_d3dSettings.IsWindowed )
        m_d3dSettings.pWindowed_AdapterInfo = pAdapterInfo;
    else
        m_d3dSettings.pFullscreen_AdapterInfo = pAdapterInfo;

    // ��ġ �޺� ���� ������Ʈ
    ComboBoxClear( IDC_DEVICE_COMBO );
    for( UINT idi = 0; idi < pAdapterInfo->pDeviceInfoList->Count(); idi++ )
    {
        D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)pAdapterInfo->pDeviceInfoList->GetPtr(idi);
        ComboBoxAdd( IDC_DEVICE_COMBO, pDeviceInfo, 
                     D3DDevTypeToString( pDeviceInfo->DevType ) );
        if( pDeviceInfo->DevType == m_d3dSettings.DevType() )
            ComboBoxSelect( IDC_DEVICE_COMBO, pDeviceInfo );
    }
    if( !ComboBoxSomethingSelected( IDC_DEVICE_COMBO ) && ComboBoxCount( IDC_DEVICE_COMBO ) > 0 )
        ComboBoxSelectIndex( IDC_DEVICE_COMBO, 0 );
}

//������ ��ġ�� ���濡 �����Ϸ���, ��ü ȭ�� / â ��� ���� ��ư.�� ��ư�� ������Ʈ�ϸ� ������ ��ȭ ������ ������Ʈ�� Ʈ�����մϴ�.
void CD3DSettingsDialog::DeviceChanged( void )
{
    D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)ComboBoxSelected( IDC_DEVICE_COMBO );
    if( pDeviceInfo == NULL )
        return;

    if( m_d3dSettings.IsWindowed )
        m_d3dSettings.pWindowed_DeviceInfo = pDeviceInfo;
    else
        m_d3dSettings.pFullscreen_DeviceInfo = pDeviceInfo;

    // ��ü ȭ��/â ��� ���� ��ư ������Ʈ
    bool HasWindowedDeviceCombo = false;
    bool HasFullscreenDeviceCombo = false;
    for( UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++ )
    {
        D3DDeviceCombo* pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
        if( pDeviceCombo->IsWindowed )
            HasWindowedDeviceCombo = true;
        else
            HasFullscreenDeviceCombo = true;
    }
    EnableWindow( GetDlgItem( m_hDlg, IDC_WINDOW ), HasWindowedDeviceCombo );
    EnableWindow( GetDlgItem( m_hDlg, IDC_FULLSCREEN ), HasFullscreenDeviceCombo );
    if (m_d3dSettings.IsWindowed && HasWindowedDeviceCombo)
        CheckRadioButton( m_hDlg, IDC_WINDOW, IDC_FULLSCREEN, IDC_WINDOW );
  
    else
        CheckRadioButton( m_hDlg, IDC_WINDOW, IDC_FULLSCREEN, IDC_FULLSCREEN );
    
    WindowedFullscreenChanged();
}

//â ��� / ��ü ȭ�� ���� ���濡 �����Ϸ���,����� ���� ���, �ػ� ���, ���� ��ħ �� ����Դϴ�.
//������ ����� ������ ������Ʈ�ϸ� ������ ��ȭ����.
void CD3DSettingsDialog::WindowedFullscreenChanged( void )
{
    D3DAdapterInfo* pAdapterInfo = (D3DAdapterInfo*)ComboBoxSelected( IDC_ADAPTER_COMBO );
    D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)ComboBoxSelected( IDC_DEVICE_COMBO );
    if( pAdapterInfo == NULL || pDeviceInfo == NULL )
        return;

    if( IsDlgButtonChecked( m_hDlg, IDC_WINDOW ) )
    {
        m_d3dSettings.IsWindowed = true;
        m_d3dSettings.pWindowed_AdapterInfo = pAdapterInfo;
        m_d3dSettings.pWindowed_DeviceInfo = pDeviceInfo;

        // ����� ���� ������Ʈ �޺� ����
        ComboBoxClear( IDC_ADAPTERFORMAT_COMBO );
        ComboBoxAdd( IDC_ADAPTERFORMAT_COMBO, (void*)m_d3dSettings.Windowed_DisplayMode.Format,
            D3DUtil_D3DFormatToString( m_d3dSettings.Windowed_DisplayMode.Format ) );
        ComboBoxSelectIndex( IDC_ADAPTERFORMAT_COMBO, 0 );
        EnableWindow( GetDlgItem( m_hDlg, IDC_ADAPTERFORMAT_COMBO ), false );

        // �ػ� �޺� ���� ������Ʈ
        DWORD dwResolutionData;
        TCHAR strResolution[50];
        dwResolutionData = MAKELONG( m_d3dSettings.Windowed_DisplayMode.Width,
                                     m_d3dSettings.Windowed_DisplayMode.Height );
        _sntprintf( strResolution, 50, TEXT("%d by %d"), m_d3dSettings.Windowed_DisplayMode.Width, 
            m_d3dSettings.Windowed_DisplayMode.Height );
        strResolution[49] = 0;
        ComboBoxClear( IDC_RESOLUTION_COMBO );
        ComboBoxAdd( IDC_RESOLUTION_COMBO, ULongToPtr(dwResolutionData), strResolution );
        ComboBoxSelectIndex( IDC_RESOLUTION_COMBO, 0 );
        EnableWindow( GetDlgItem( m_hDlg, IDC_RESOLUTION_COMBO ), false );

        // ���� ��ħ �� �޺� ���� ������Ʈ
        TCHAR strRefreshRate[50];
        if( m_d3dSettings.Windowed_DisplayMode.RefreshRate == 0 )
            lstrcpy( strRefreshRate, TEXT("Default Rate") );
        else
            _sntprintf( strRefreshRate, 50, TEXT("%d Hz"), m_d3dSettings.Windowed_DisplayMode.RefreshRate );
        strRefreshRate[49] = 0;
        ComboBoxClear( IDC_REFRESHRATE_COMBO );
        ComboBoxAdd( IDC_REFRESHRATE_COMBO, ULongToPtr(m_d3dSettings.Windowed_DisplayMode.RefreshRate),
            strRefreshRate );
        ComboBoxSelectIndex( IDC_REFRESHRATE_COMBO, 0 );
        EnableWindow( GetDlgItem( m_hDlg, IDC_REFRESHRATE_COMBO ), false );
    }
    else
    {
        m_d3dSettings.IsWindowed = false;
        m_d3dSettings.pFullscreen_AdapterInfo = pAdapterInfo;
        m_d3dSettings.pFullscreen_DeviceInfo = pDeviceInfo;

        // ����� ���� ������Ʈ �޺� ����
        ComboBoxClear( IDC_ADAPTERFORMAT_COMBO );
        for( UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++ )
        {
            D3DDeviceCombo* pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
            D3DFORMAT adapterFormat = pDeviceCombo->AdapterFormat;
            if( !ComboBoxContainsText( IDC_ADAPTERFORMAT_COMBO, D3DUtil_D3DFormatToString( adapterFormat ) ) )
            {
                ComboBoxAdd( IDC_ADAPTERFORMAT_COMBO, (void*)adapterFormat, 
                    D3DUtil_D3DFormatToString( adapterFormat ) );
                if( adapterFormat == m_d3dSettings.Fullscreen_DisplayMode.Format )
                    ComboBoxSelect( IDC_ADAPTERFORMAT_COMBO, (void*)adapterFormat );
                
            }
        }
        if( !ComboBoxSomethingSelected( IDC_ADAPTERFORMAT_COMBO ) && ComboBoxCount( IDC_ADAPTERFORMAT_COMBO ) > 0 )
            ComboBoxSelectIndex( IDC_ADAPTERFORMAT_COMBO, 0 );
        
        EnableWindow( GetDlgItem( m_hDlg, IDC_ADAPTERFORMAT_COMBO), true );
        
        // �ػ� �޺� ���� ������Ʈ
        EnableWindow( GetDlgItem( m_hDlg, IDC_RESOLUTION_COMBO), true );
        
        // ���� ��ħ �� �޺� ���� ������Ʈ
        EnableWindow( GetDlgItem( m_hDlg, IDC_REFRESHRATE_COMBO), true );
    }
}

//������ ����� ������ ���濡 �����Ϸ���, ������ �׸� ������Ʈ �� �ػ� ��ϰ� ����� ���� ���.  
//�ػ� �� �� ���� ������ ������ ������Ʈ�� Ʈ�����մϴ�. 
//��ȭ����.
void CD3DSettingsDialog::AdapterFormatChanged( void )
{
    if( !IsDlgButtonChecked( m_hDlg, IDC_WINDOW ) )
    {
        D3DAdapterInfo* pAdapterInfo = (D3DAdapterInfo*)ComboBoxSelected( IDC_ADAPTER_COMBO );
        D3DFORMAT adapterFormat = (D3DFORMAT)PtrToUlong( ComboBoxSelected( IDC_ADAPTERFORMAT_COMBO ) );
        m_d3dSettings.Fullscreen_DisplayMode.Format = adapterFormat;

        ComboBoxClear( IDC_RESOLUTION_COMBO );
        for( UINT idm = 0; idm < pAdapterInfo->pDisplayModeList->Count(); idm++ )
        {
            D3DDISPLAYMODE displayMode = *(D3DDISPLAYMODE*)pAdapterInfo->pDisplayModeList->GetPtr(idm);
            if (displayMode.Format == adapterFormat)
            {
                DWORD dwResolutionData;
                TCHAR strResolution[50];
                dwResolutionData = MAKELONG( displayMode.Width, displayMode.Height );
                _sntprintf( strResolution, 50, TEXT("%d by %d"), displayMode.Width, displayMode.Height );
                strResolution[49] = 0;
                if (!ComboBoxContainsText( IDC_RESOLUTION_COMBO, strResolution ) )
                {
                    ComboBoxAdd( IDC_RESOLUTION_COMBO, ULongToPtr( dwResolutionData ), strResolution );
                    if (m_d3dSettings.Fullscreen_DisplayMode.Width == displayMode.Width && m_d3dSettings.Fullscreen_DisplayMode.Height == displayMode.Height)
                        ComboBoxSelect( IDC_RESOLUTION_COMBO, ULongToPtr( dwResolutionData ) );
                }
            }
        }
        if (!ComboBoxSomethingSelected( IDC_RESOLUTION_COMBO ) &&  ComboBoxCount( IDC_RESOLUTION_COMBO ) > 0)
            ComboBoxSelectIndex( IDC_RESOLUTION_COMBO, 0 );
    }

    // ����� ���� ������Ʈ �޺� ����
    D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)ComboBoxSelected( IDC_DEVICE_COMBO );
    if( pDeviceInfo == NULL )
        return;
    ComboBoxClear( IDC_BACKBUFFERFORMAT_COMBO );
    for( UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++ )
    {
        D3DDeviceCombo* pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
        if (pDeviceCombo->IsWindowed == m_d3dSettings.IsWindowed &&
            pDeviceCombo->AdapterFormat == m_d3dSettings.DisplayMode().Format)
        {
            if (!ComboBoxContainsText( IDC_BACKBUFFERFORMAT_COMBO, D3DUtil_D3DFormatToString( pDeviceCombo->BackBufferFormat ) ) )
            {
                ComboBoxAdd( IDC_BACKBUFFERFORMAT_COMBO, (void*)pDeviceCombo->BackBufferFormat,
                    D3DUtil_D3DFormatToString( pDeviceCombo->BackBufferFormat ) );
                if (pDeviceCombo->BackBufferFormat == m_d3dSettings.BackBufferFormat() )
                    ComboBoxSelect( IDC_BACKBUFFERFORMAT_COMBO, (void*)pDeviceCombo->BackBufferFormat );
            }
        }
    }
    if (!ComboBoxSomethingSelected( IDC_BACKBUFFERFORMAT_COMBO ) &&  ComboBoxCount( IDC_BACKBUFFERFORMAT_COMBO ) > 0)
        ComboBoxSelectIndex( IDC_BACKBUFFERFORMAT_COMBO, 0 );
}

//������ �ػ��� ���濡 �����Ϸ���, ���� ��ħ �� ���.
void CD3DSettingsDialog::ResolutionChanged( void )
{
    if (m_d3dSettings.IsWindowed)
        return;

    D3DAdapterInfo* pAdapterInfo = (D3DAdapterInfo*)ComboBoxSelected( IDC_ADAPTER_COMBO );
    if( pAdapterInfo == NULL )
        return;

    // ���ο� �ػ󵵷� settingsNew ������Ʈ
    DWORD dwResolutionData = PtrToUlong( ComboBoxSelected( IDC_RESOLUTION_COMBO ) );
    UINT width = LOWORD( dwResolutionData );
    UINT height = HIWORD( dwResolutionData );
    m_d3dSettings.Fullscreen_DisplayMode.Width = width;
    m_d3dSettings.Fullscreen_DisplayMode.Height = height;

    // ���ο� �ػ󵵿� ���� ���� ��ħ �� ����� ������Ʈ�մϴ�.
    D3DFORMAT adapterFormat = (D3DFORMAT)PtrToUlong( ComboBoxSelected( IDC_ADAPTERFORMAT_COMBO ) );
    ComboBoxClear( IDC_REFRESHRATE_COMBO );
    for( UINT idm = 0; idm < pAdapterInfo->pDisplayModeList->Count(); idm++ )
    {
        D3DDISPLAYMODE displayMode = *(D3DDISPLAYMODE*)pAdapterInfo->pDisplayModeList->GetPtr(idm);
        if (displayMode.Format == adapterFormat && displayMode.Width  == width && displayMode.Height == height)
        {
            TCHAR strRefreshRate[50];
            if( displayMode.RefreshRate == 0 )
                lstrcpy( strRefreshRate, TEXT("Default Rate") );
            else
                _sntprintf( strRefreshRate, 50, TEXT("%d Hz"), displayMode.RefreshRate );
            strRefreshRate[49] = 0;
            if( !ComboBoxContainsText( IDC_REFRESHRATE_COMBO, strRefreshRate ) )
            {
                ComboBoxAdd( IDC_REFRESHRATE_COMBO, UlongToPtr( displayMode.RefreshRate ), strRefreshRate );
                if (m_d3dSettings.Fullscreen_DisplayMode.RefreshRate == displayMode.RefreshRate)
                    ComboBoxSelect( IDC_REFRESHRATE_COMBO, UlongToPtr( displayMode.RefreshRate ) );
            }
        }
    }
    if (!ComboBoxSomethingSelected( IDC_REFRESHRATE_COMBO ) &&  ComboBoxCount( IDC_REFRESHRATE_COMBO ) > 0)
        ComboBoxSelectIndex( IDC_REFRESHRATE_COMBO, 0 );
    
}

//������ ���� ��ħ ���� ���濡 �����մϴ�.
void CD3DSettingsDialog::RefreshRateChanged( void )
{
    if( m_d3dSettings.IsWindowed )
        return;

    // Update settingsNew with new refresh rate
    UINT refreshRate = PtrToUlong( ComboBoxSelected( IDC_REFRESHRATE_COMBO ) );
    m_d3dSettings.Fullscreen_DisplayMode.RefreshRate = refreshRate;
}

// �籸���� ���� ������ ����� ���� ���濡 ����
// ����/���ٽ� ���� ���, ���� ���� ���� ��� �� ����
// ó�� ���� ���.
void CD3DSettingsDialog::BackBufferFormatChanged( void )
{
    D3DDeviceInfo* pDeviceInfo = (D3DDeviceInfo*)ComboBoxSelected( IDC_DEVICE_COMBO );
    D3DFORMAT adapterFormat = (D3DFORMAT)PtrToUlong( ComboBoxSelected( IDC_ADAPTERFORMAT_COMBO ) );
    D3DFORMAT backBufferFormat = (D3DFORMAT)PtrToUlong( ComboBoxSelected( IDC_BACKBUFFERFORMAT_COMBO ) );
    if( pDeviceInfo == NULL )
        return;

    for( UINT idc = 0; idc < pDeviceInfo->pDeviceComboList->Count(); idc++ )
    {
        D3DDeviceCombo* pDeviceCombo = (D3DDeviceCombo*)pDeviceInfo->pDeviceComboList->GetPtr(idc);
        if (pDeviceCombo->IsWindowed == m_d3dSettings.IsWindowed && pDeviceCombo->AdapterFormat == adapterFormat && pDeviceCombo->BackBufferFormat == backBufferFormat)
        {
            if( m_d3dSettings.IsWindowed )
                m_d3dSettings.pWindowed_DeviceCombo = pDeviceCombo;
            else
                m_d3dSettings.pFullscreen_DeviceCombo = pDeviceCombo;

            ComboBoxClear( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO );
            if( m_pEnumeration->AppUsesDepthBuffer )
            {
                for( UINT ifmt = 0; ifmt < pDeviceCombo->pDepthStencilFormatList->Count(); ifmt++ )
                {
                    D3DFORMAT fmt = *(D3DFORMAT*)pDeviceCombo->pDepthStencilFormatList->GetPtr(ifmt);
                    ComboBoxAdd( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO, (void*)fmt, 
                        D3DUtil_D3DFormatToString(fmt) );
                    if( fmt == m_d3dSettings.DepthStencilBufferFormat() )
                        ComboBoxSelect( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO, (void*)fmt );
                }
                if (!ComboBoxSomethingSelected( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO ) &&  ComboBoxCount( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO ) > 0)
                    ComboBoxSelectIndex( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO, 0 );
                
            }
            else
            {
                EnableWindow( GetDlgItem( m_hDlg, IDC_DEPTHSTENCILBUFFERFORMAT_COMBO ), false );
                ComboBoxAdd( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO, NULL, TEXT("(not used)") );
                ComboBoxSelectIndex( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO, 0 );
            }

            ComboBoxClear( IDC_VERTEXPROCESSING_COMBO );
            for( UINT ivpt = 0; ivpt < pDeviceCombo->pVertexProcessingTypeList->Count(); ivpt++ )
            {
                VertexProcessingType vpt = *(VertexProcessingType*)pDeviceCombo->pVertexProcessingTypeList->GetPtr(ivpt);
                ComboBoxAdd( IDC_VERTEXPROCESSING_COMBO, (void*)vpt, VertexProcessingTypeToString(vpt) );
                if( vpt == m_d3dSettings.GetVertexProcessingType() )
                    ComboBoxSelect( IDC_VERTEXPROCESSING_COMBO, (void*)vpt );
            }
            if (!ComboBoxSomethingSelected( IDC_VERTEXPROCESSING_COMBO ) &&  ComboBoxCount( IDC_VERTEXPROCESSING_COMBO ) > 0)
                ComboBoxSelectIndex( IDC_VERTEXPROCESSING_COMBO, 0 );

            ComboBoxClear( IDC_PRESENTINTERVAL_COMBO );
            for( UINT ipi = 0; ipi < pDeviceCombo->pPresentIntervalList->Count(); ipi++ )
            {
                UINT pi = *(UINT*)pDeviceCombo->pPresentIntervalList->GetPtr(ipi);
                ComboBoxAdd( IDC_PRESENTINTERVAL_COMBO, UlongToPtr( pi ), PresentIntervalToString(pi) );
                if( pi == m_d3dSettings.PresentInterval() )
                    ComboBoxSelect( IDC_PRESENTINTERVAL_COMBO, UlongToPtr( pi ) );
            }
            if (!ComboBoxSomethingSelected( IDC_PRESENTINTERVAL_COMBO ) && ComboBoxCount( IDC_PRESENTINTERVAL_COMBO ) > 0)
                ComboBoxSelectIndex( IDC_PRESENTINTERVAL_COMBO, 0 );
            break;
        }
    }
}

//������ ���� / ���ٽ� ���� ������ ���濡 �����մϴ�.
void CD3DSettingsDialog::DepthStencilBufferFormatChanged( void )
{
    D3DFORMAT fmt = (D3DFORMAT)PtrToUlong( ComboBoxSelected( IDC_DEPTHSTENCILBUFFERFORMAT_COMBO ) );
    if( m_pEnumeration->AppUsesDepthBuffer )
        m_d3dSettings.SetDepthStencilBufferFormat( fmt );

    // ���� ���� ��� �ۼ�
    D3DDeviceCombo* pDeviceCombo = m_d3dSettings.PDeviceCombo();
    ComboBoxClear( IDC_MULTISAMPLE_COMBO );
    for( UINT ims = 0; ims < pDeviceCombo->pMultiSampleTypeList->Count(); ims++ )
    {
        D3DMULTISAMPLE_TYPE msType = *(D3DMULTISAMPLE_TYPE*)pDeviceCombo->pMultiSampleTypeList->GetPtr(ims);

        // DS/MS �浹�� Ȯ���մϴ�.
        BOOL bConflictFound = FALSE;
        for( UINT iConf = 0; iConf < pDeviceCombo->pDSMSConflictList->Count(); iConf++ )
        {
            D3DDSMSConflict* pDSMSConf = (D3DDSMSConflict*)pDeviceCombo->pDSMSConflictList->GetPtr(iConf);
            if( pDSMSConf->DSFormat == fmt && pDSMSConf->MSType == msType )
            {
                bConflictFound = TRUE;
                break;
            }
        }
        if( !bConflictFound )
        {
            ComboBoxAdd( IDC_MULTISAMPLE_COMBO, (void*)msType, MultisampleTypeToString(msType) );
            if( msType == m_d3dSettings.MultisampleType() )
                ComboBoxSelect( IDC_MULTISAMPLE_COMBO, (void*)msType );
        }
    }
    if (!ComboBoxSomethingSelected( IDC_MULTISAMPLE_COMBO ) &&  ComboBoxCount( IDC_MULTISAMPLE_COMBO ) > 0)
        ComboBoxSelectIndex( IDC_MULTISAMPLE_COMBO, 0 );
}

//������ ���� ���� ������ ���濡 �����մϴ�.
void CD3DSettingsDialog::MultisampleTypeChanged( void )
{
    D3DMULTISAMPLE_TYPE mst = (D3DMULTISAMPLE_TYPE)PtrToUlong( ComboBoxSelected( IDC_MULTISAMPLE_COMBO ) );
    m_d3dSettings.SetMultisampleType( mst );

    // �� mst�� ���� �ִ� ǰ���� �����մϴ�.
    D3DDeviceCombo* pDeviceCombo = m_d3dSettings.PDeviceCombo();
    DWORD maxQuality = 0;

    for( UINT ims = 0; ims < pDeviceCombo->pMultiSampleTypeList->Count(); ims++ )
    {
        D3DMULTISAMPLE_TYPE msType = *(D3DMULTISAMPLE_TYPE*)pDeviceCombo->pMultiSampleTypeList->GetPtr(ims);
        if( msType == mst )
        {
            maxQuality = *(DWORD*)pDeviceCombo->pMultiSampleQualityList->GetPtr(ims);
            break;
        }
    }

    ComboBoxClear( IDC_MULTISAMPLE_QUALITY_COMBO );
    for( UINT msq = 0; msq < maxQuality; msq++ )
    {
        TCHAR str[100];
        wsprintf( str, TEXT("%d"), msq );
        ComboBoxAdd( IDC_MULTISAMPLE_QUALITY_COMBO, UlongToPtr( msq ), str );
        if( msq == m_d3dSettings.MultisampleQuality() )
            ComboBoxSelect( IDC_MULTISAMPLE_QUALITY_COMBO, UlongToPtr( msq ) );
    }
    if (!ComboBoxSomethingSelected( IDC_MULTISAMPLE_QUALITY_COMBO ) && ComboBoxCount( IDC_MULTISAMPLE_QUALITY_COMBO ) > 0)
        ComboBoxSelectIndex( IDC_MULTISAMPLE_QUALITY_COMBO, 0 );
}

//������ ���� ���� ǰ���� ���濡 �����մϴ�.
void CD3DSettingsDialog::MultisampleQualityChanged( void )
{
    DWORD msq = (DWORD)PtrToUlong( ComboBoxSelected( IDC_MULTISAMPLE_QUALITY_COMBO ) );
    m_d3dSettings.SetMultisampleQuality( msq );
}

//���õ� ���� ó�� ���� ���濡 ����
void CD3DSettingsDialog::VertexProcessingChanged( void )
{
    VertexProcessingType vpt = (VertexProcessingType)PtrToUlong( ComboBoxSelected( IDC_VERTEXPROCESSING_COMBO ) );
    m_d3dSettings.SetVertexProcessingType( vpt );
}

//������ ���� ������ ���濡 �����մϴ�.
void CD3DSettingsDialog::PresentIntervalChanged( void )
{
    UINT pi = PtrToUlong( ComboBoxSelected( IDC_PRESENTINTERVAL_COMBO ) );
    m_d3dSettings.SetPresentInterval( pi );
}