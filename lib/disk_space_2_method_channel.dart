import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'disk_space_2_platform_interface.dart';

/// An implementation of [DiskSpace_2Platform] that uses method channels.
class MethodChannelDiskSpace_2 extends DiskSpace_2Platform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('disk_space_2');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }
}
