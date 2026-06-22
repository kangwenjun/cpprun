#include <windows.h>
#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "wlanapi.lib")

bool ConnectToWifi(const std::wstring& ssid, const std::wstring& password) 
{
    HANDLE hClient = NULL;
    DWORD dwMaxClient = 2;
    DWORD dwCurVersion = 0;
    DWORD dwResult = 0;

    // 初始化WLAN API
    dwResult = WlanOpenHandle(dwMaxClient, NULL, &dwCurVersion, &hClient);
    if (dwResult != ERROR_SUCCESS) {
        std::cerr << "WlanOpenHandle failed: " << dwResult << std::endl;
        return false;
    }

    // 获取可用网络列表
    PWLAN_INTERFACE_INFO_LIST pIfList = NULL;
    dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
    if (dwResult != ERROR_SUCCESS) {
        std::cerr << "WlanEnumInterfaces failed: " << dwResult << std::endl;
        WlanCloseHandle(hClient, NULL);
        return false;
    }

    bool connected = false;
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

    for (DWORD i = 0; i < pIfList->dwNumberOfItems; i++) {
        PWLAN_INTERFACE_INFO pIfInfo = &pIfList->InterfaceInfo[i];
        const GUID* pInterfaceGuid = &pIfInfo->InterfaceGuid;

        // 扫描网络（可选）
        WlanScan(hClient, pInterfaceGuid, NULL, NULL, NULL);

        // 获取网络列表
        PWLAN_AVAILABLE_NETWORK_LIST pAvailableNetworkList = NULL;
        dwResult = WlanGetAvailableNetworkList(hClient, pInterfaceGuid, 0, NULL, &pAvailableNetworkList);
        if (dwResult == ERROR_SUCCESS) {
            for (DWORD j = 0; j < pAvailableNetworkList->dwNumberOfItems; j++) {
                PWLAN_AVAILABLE_NETWORK pNetwork = &pAvailableNetworkList->Network[j];
                std::wstring networkSsid = ConvertDot11SsidToWString(pNetwork->dot11Ssid);
                if (networkSsid == ssid) {
                    std::wcout << L"Found network: " << networkSsid << std::endl;

                    auto EscapeXml = [](const std::wstring& s) -> std::wstring {
                        std::wstring out;
                        out.reserve(s.size());
                        for (wchar_t c : s) {
                            switch (c) {
                                case L'&': out += L"&amp;"; break;
                                case L'<': out += L"&lt;"; break;
                                case L'>': out += L"&gt;"; break;
                                case L'\"': out += L"&quot;"; break;
                                case L'\'': out += L"&apos;"; break;
                                default: out += c; break;
                            }
                        }
                        return out;
                    };

                    std::wstring escSsid = EscapeXml(ssid);
                    std::wstring escPassword = EscapeXml(password);
                    std::wstring profileXml =
                        L"<?xml version=\"1.0\"?>"
                        L"<WLANProfile xmlns=\"http://www.microsoft.com/networking/WLAN/profile/v1\">"
                        L"<name>" + escSsid + L"</name>"
                        L"<SSIDConfig><SSID><name>" + escSsid + L"</name></SSID></SSIDConfig>"
                        L"<connectionType>ESS</connectionType>"
                        L"<connectionMode>auto</connectionMode>"
                        L"<MSM><security>"
                        L"<authEncryption>"
                        L"<authentication>WPA2PSK</authentication>"
                        L"<encryption>AES</encryption>"
                        L"<useOneX>false</useOneX>"
                        L"</authEncryption>"
                        L"<sharedKey>"
                        L"<keyType>passPhrase</keyType>"
                        L"<protected>false</protected>"
                        L"<keyMaterial>" + escPassword + L"</keyMaterial>"
                        L"</sharedKey>"
                        L"</security></MSM>"
                        L"</WLANProfile>";

                    DWORD pdwReason = 0;
                    dwResult = WlanSetProfile(hClient, pInterfaceGuid, 0, profileXml.c_str(), NULL, TRUE, NULL, &pdwReason);
                    if (dwResult != ERROR_SUCCESS) {
                        std::cerr << "WlanSetProfile failed: " << dwResult << " reason: " << pdwReason << std::endl;
                    } else {
                        std::cout << "WlanSetProfile succeeded for profile: " << std::string(escSsid.begin(), escSsid.end()) << std::endl;
                    }

                    // 列出现有 profile，确认 profile 名称存在
                    PWLAN_PROFILE_INFO_LIST pProfileList = NULL;
                    dwResult = WlanGetProfileList(hClient, pInterfaceGuid, NULL, &pProfileList);
                    if (dwResult == ERROR_SUCCESS && pProfileList) {
                        bool foundProfile = false;
                        for (DWORD pi = 0; pi < pProfileList->dwNumberOfItems; ++pi) {
                            PWLAN_PROFILE_INFO pinfo = &pProfileList->ProfileInfo[pi];
                            std::wstring pname = pinfo->strProfileName;
                            std::wcout << L"Profile: " << pname << std::endl;
                            if (pname == ssid) foundProfile = true;
                        }
                        if (!foundProfile) std::wcerr << L"Profile not found after WlanSetProfile: " << ssid << std::endl;
                        WlanFreeMemory(pProfileList);
                    } else {
                        std::cerr << "WlanGetProfileList failed: " << dwResult << std::endl;
                    }

                    // 再次尝试连接
                    WLAN_CONNECTION_PARAMETERS connectionParams;
                    ZeroMemory(&connectionParams, sizeof(connectionParams));
                    connectionParams.wlanConnectionMode = wlan_connection_mode_profile;
                    connectionParams.strProfile = const_cast<LPWSTR>(ssid.c_str());
                    connectionParams.dwFlags = 0;
                    connectionParams.pDot11Ssid = NULL;
                    connectionParams.pDesiredBssidList = NULL;
                    connectionParams.dot11BssType = dot11_BSS_type_infrastructure;

                    dwResult = WlanConnect(hClient, pInterfaceGuid, &connectionParams, NULL);
                    if (dwResult == ERROR_SUCCESS) {
                        connected = true;
                    } else {
                        std::cerr << "WlanConnect failed: " << dwResult << std::endl;
                    }
                    break;
                }
            }
            if (pAvailableNetworkList) WlanFreeMemory(pAvailableNetworkList);
        }
        if (connected) break;
    }

    if (pIfList) WlanFreeMemory(pIfList);
    WlanCloseHandle(hClient, NULL);
    return connected;
}


#ifdef __MAIN__
#include <iostream>
#include "wifi_connection.h"

static std::wstring ToWString(const char* s) {
    if (!s) return L"";
    int required = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
    if (required > 0) {
        std::wstring out;
        out.resize(required - 1);
        MultiByteToWideChar(CP_UTF8, 0, s, -1, &out[0], required);
        return out;
    }
    required = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
    if (required > 0) {
        std::wstring out;
        out.resize(required - 1);
        MultiByteToWideChar(CP_ACP, 0, s, -1, &out[0], required);
        return out;
    }
    return L"";
}

int main(int argc, char** argv)
{
    std::wstring ssid;
    std::wstring password;

    if (argc >= 2) {
        ssid = ToWString(argv[1]);
    }
    if (argc >= 3) {
        password = ToWString(argv[2]);
    }

    if (ssid.empty()) {
        std::cerr << "SSID is empty. Usage: project_bin.exe <ssid> <password>" << std::endl;
        return 2;
    }

    if (password.empty()) {
        std::cerr << "Password is empty. Usage: project_bin.exe <ssid> <password>" << std::endl;
        return 2;
    }

    if (ConnectToWifi(ssid, password)) {
        std::wcout << L"Connected successfully!" << std::endl;
    } else {
        std::wcerr << L"Connection failed." << std::endl;
    }
    return 0;
}

#endif // __MAIN__