#include <iostream>
#include <string>
#include <windows.h>
#include "win_wifi_connection.h"

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
