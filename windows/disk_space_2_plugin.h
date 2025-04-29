#ifndef FLUTTER_PLUGIN_DISK_SPACE_2_PLUGIN_H_
#define FLUTTER_PLUGIN_DISK_SPACE_2_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace disk_space_2 {

class DiskSpace_2Plugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  DiskSpace_2Plugin();

  virtual ~DiskSpace_2Plugin();

  // Disallow copy and assign.
  DiskSpace_2Plugin(const DiskSpace_2Plugin&) = delete;
  DiskSpace_2Plugin& operator=(const DiskSpace_2Plugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace disk_space_2

#endif  // FLUTTER_PLUGIN_DISK_SPACE_2_PLUGIN_H_
