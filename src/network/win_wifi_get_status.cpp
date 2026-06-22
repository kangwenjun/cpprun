#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>
#include <iostream>
#include <iomanip>
#include <string>

#pragma comment(lib, "wlanapi.lib")

// 获取 Wi-Fi 连接状态
void GetWifiConnectionStatus() {
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;

    // 1. 初始化 WLAN API
    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        std::cerr << "WlanOpenHandle failed: " << dwResult << std::endl;
        return;
    }

    // 2. 获取所有无线网卡接口
    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        std::cerr << "WlanEnumInterfaces failed: " << dwResult << std::endl;
        WlanCloseHandle(hClient, NULL);
        return;
    }

    // helper: convert DOT11_SSID to wstring
    auto ConvertDot11SsidToWString = [](const DOT11_SSID& dssid) -> std::wstring {
        if (dssid.uSSIDLength == 0) return L"";
        int required = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(dssid.ucSSID), dssid.uSSIDLength, NULL, 0);
        std::wstring out;
        if (required > 0) {
            out.resize(required);
            MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(dssid.ucSSID), dssid.uSSIDLength, &out[0], required);
            return out;
        }
        required = MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<const char*>(dssid.ucSSID), dssid.uSSIDLength, NULL, 0);
        if (required > 0) {
            out.resize(required);
            MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<const char*>(dssid.ucSSID), dssid.uSSIDLength, &out[0], required);
            return out;
        }
        return L"";
    };

    // 3. 遍历所有无线网卡
    for (DWORD i = 0; i < pIfList->dwNumberOfItems; i++) {
        PWLAN_INTERFACE_INFO pIfInfo = &pIfList->InterfaceInfo[i];
        const GUID* pInterfaceGuid = &pIfInfo->InterfaceGuid;

        // 4. 获取当前连接状态
        PWLAN_CONNECTION_ATTRIBUTES pConnInfo = NULL;
        DWORD dwInfoSize = sizeof(WLAN_CONNECTION_ATTRIBUTES);
        dwResult = WlanQueryInterface(
            hClient,
            pInterfaceGuid,
            wlan_intf_opcode_current_connection,
            NULL,
            &dwInfoSize,
            (PVOID*)&pConnInfo,
            NULL
        );

        if (dwResult == ERROR_SUCCESS && pConnInfo != NULL) {
            std::wcout << L"\n=== Wi-Fi 接口: " << pIfInfo->strInterfaceDescription << L" ===" << std::endl;

            // 检查是否已连接
            if (pConnInfo->isState == wlan_interface_state_connected) {
                std::wcout << L"状态: 已连接" << std::endl;
                std::wstring ssid = ConvertDot11SsidToWString(pConnInfo->wlanAssociationAttributes.dot11Ssid);
                std::wcout << L"SSID: " << ssid << std::endl;
                std::wcout << L"BSSID: " << std::hex << std::setw(2) << std::setfill(L'0')
                          << (int)pConnInfo->wlanAssociationAttributes.dot11Bssid[0] << L":";
                for (int j = 1; j < 6; j++) {
                    std::wcout << std::hex << std::setw(2) << std::setfill(L'0')
                              << (int)pConnInfo->wlanAssociationAttributes.dot11Bssid[j];
                    if (j < 5) std::wcout << L":";
                }
                std::wcout << std::dec << std::endl;
                // 获取信号强度（使用当前连接信息中的 wlanSignalQuality 字段）
                DWORD signalQuality = pConnInfo->wlanAssociationAttributes.wlanSignalQuality;
                std::wcout << L"信号强度: " << signalQuality << L"%" << std::endl;
            } else {
                std::wcout << L"状态: 未连接" << std::endl;
            }

            // 释放内存
            if (pConnInfo) WlanFreeMemory(pConnInfo);
        } else {
            std::wcout << L"无法获取连接信息。" << std::endl;
        }
    }

    // 5. 释放资源
    if (pIfList) WlanFreeMemory(pIfList);
    WlanCloseHandle(hClient, NULL);
}

int main() {
    GetWifiConnectionStatus();
    return 0;
}
