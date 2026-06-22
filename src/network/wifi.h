#pragma once

#include <string>

bool ConnectToWifi(const std::wstring& ssid, const std::wstring& password);
void GetWifiConnectionStatus();