#include "include/disk_space_2/disk_space_2_plugin.h"
#include "include/disk_space_2/disk_space_2_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "disk_space_2_plugin.h"

void DiskSpace_2PluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  disk_space_2::DiskSpace_2Plugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}

void DiskSpace_2PluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  disk_space_2::DiskSpace_2Plugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
