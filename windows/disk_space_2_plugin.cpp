#include "disk_space_2_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include <flutter_windows.h>
#include <KnownFolders.h>
#include <shlobj_core.h>

#include <codecvt>
#include <memory>
#include <sstream>
#include <string>

namespace disk_space_2 {

constexpr char kBadArgumentsError[] = "Bad Arguments";
constexpr char kNoMemory[] = "Out of memory";
constexpr char PATH_KEY[] = "path";
constexpr double ONE_MEGABYTE = 1024 * 1024;

// Helper function to get detailed Windows error message
std::string GetLastErrorAsString() {
    DWORD error = GetLastError();
    if (error == 0) {
        return std::string();
    }

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        NULL
    );

    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}

// Helper function to validate path
bool IsValidPath(const std::wstring& path) {
    if (path.empty()) return false;
    
    // Check if path exists and is accessible
    DWORD attributes = GetFileAttributesW(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    
    // Check if it's a directory
    if (!(attributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return false;
    }
    
    return true;
}

enum class WhichMethod {
  UNKNOWN,
  FREE_SPACE_PATH,
  TOTAL_SPACE_PATH,
  FREE_SPACE,
  TOTAL_SPACE,
  PLATFORM_VER,
};

// static
void DiskSpace_2Plugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "disk_space_2",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<DiskSpace_2Plugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

DiskSpace_2Plugin::DiskSpace_2Plugin() {}

DiskSpace_2Plugin::~DiskSpace_2Plugin() {}

void DiskSpace_2Plugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  auto whichMethod = WhichMethod::UNKNOWN;
  if (method_call.method_name().compare("getFreeDiskSpaceForPath") == 0) {
    whichMethod = WhichMethod::FREE_SPACE_PATH;
  } else if (method_call.method_name().compare("getTotalDiskSpaceForPath") == 0) {
    whichMethod = WhichMethod::TOTAL_SPACE_PATH;
  } else if (method_call.method_name().compare("getFreeDiskSpace") == 0) {
    whichMethod = WhichMethod::FREE_SPACE;
  } else if (method_call.method_name().compare("getTotalDiskSpace") == 0) {
    whichMethod = WhichMethod::TOTAL_SPACE;
  } else if (method_call.method_name().compare("getPlatformVersion") == 0) {
    whichMethod = WhichMethod::PLATFORM_VER;
  }

  switch (whichMethod) {
    case WhichMethod::FREE_SPACE_PATH:
    case WhichMethod::TOTAL_SPACE_PATH: {
      const auto* args = std::get_if<flutter::EncodableMap>(method_call.arguments());
      if (!args) {
        result->Error(kBadArgumentsError, "Expected string");
        return;
      }
      auto path_it = args->find(flutter::EncodableValue(PATH_KEY));
      if (path_it == args->end()) {
        result->Error(kBadArgumentsError, "Expected 'path' argument");
        return;
      }
      if (!(std::holds_alternative<std::string>(path_it->second))) {
        result->Error(kBadArgumentsError, "Expected string in Map entry");
        return;
      }
      auto path = std::get<std::string>(path_it->second);
      std::wstring pathStr = std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(path);
      
      // Validate path
      if (!IsValidPath(pathStr)) {
        result->Error("InvalidPath", "The provided path is invalid or inaccessible");
        return;
      }

      // Retrieve free disk space
      ULARGE_INTEGER lFreeBytesAvailableToCaller;
      ULARGE_INTEGER lTotalNumberOfBytes;
      ULARGE_INTEGER lTotalNumberOfFreeBytes;
      if (!GetDiskFreeSpaceEx(pathStr.c_str(), &lFreeBytesAvailableToCaller, &lTotalNumberOfBytes, &lTotalNumberOfFreeBytes)) {
        std::string errorMsg = GetLastErrorAsString();
        result->Error("DiskSpaceError", "Failed to get disk space information: " + errorMsg);
        return;
      }
      if (whichMethod == WhichMethod::FREE_SPACE_PATH)
        result->Success(flutter::EncodableValue(static_cast<double>(lFreeBytesAvailableToCaller.QuadPart) / ONE_MEGABYTE));
      else
        result->Success(flutter::EncodableValue(static_cast<double>(lTotalNumberOfBytes.QuadPart) / ONE_MEGABYTE));
      break;
    }
    case WhichMethod::FREE_SPACE:
    case WhichMethod::TOTAL_SPACE: {
      // Unlike Apple or Android phones, Windows does not have a single "disk" that we can
      // query the properties of. Assume the user wants the properties of the partition
      // their user profile is stored on; retrieve the Desktop folder's location
      PWSTR folderPath = NULL;
      HRESULT hr = SHGetKnownFolderPath(FOLDERID_Desktop, KF_FLAG_DEFAULT, nullptr, &folderPath);
      if (S_OK != hr) {
        std::string errorMsg = GetLastErrorAsString();
        result->Error("FolderError", "Failed to get Desktop folder location: " + errorMsg);
        CoTaskMemFree(folderPath);
        return;
      }

      // Validate path
      if (!IsValidPath(folderPath)) {
        result->Error("InvalidPath", "The Desktop folder path is invalid or inaccessible");
        CoTaskMemFree(folderPath);
        return;
      }

      // Retrieve free disk space
      ULARGE_INTEGER lFreeBytesAvailableToCaller;
      ULARGE_INTEGER lTotalNumberOfBytes;
      ULARGE_INTEGER lTotalNumberOfFreeBytes;
      if (!GetDiskFreeSpaceEx(folderPath, &lFreeBytesAvailableToCaller, &lTotalNumberOfBytes, &lTotalNumberOfFreeBytes)) {
        std::string errorMsg = GetLastErrorAsString();
        result->Error("DiskSpaceError", "Failed to get disk space information: " + errorMsg);
        CoTaskMemFree(folderPath);
        return;
      }
      CoTaskMemFree(folderPath);

      if (whichMethod == WhichMethod::FREE_SPACE)
        result->Success(flutter::EncodableValue(static_cast<double>(lFreeBytesAvailableToCaller.QuadPart) / ONE_MEGABYTE));
      else
        result->Success(flutter::EncodableValue(static_cast<double>(lTotalNumberOfBytes.QuadPart) / ONE_MEGABYTE));
      break;
    }
    case WhichMethod::PLATFORM_VER: {
      std::ostringstream version_stream;
      version_stream << "Windows ";
      if (IsWindows10OrGreater()) {
        version_stream << "10+";
      } else if (IsWindows8OrGreater()) {
        version_stream << "8";
      } else if (IsWindows7OrGreater()) {
        version_stream << "7";
      }
      result->Success(flutter::EncodableValue(version_stream.str()));
      break;
    }
    default: {
      result->NotImplemented();
      break;
    }
  }
}

}  // namespace disk_space_2
