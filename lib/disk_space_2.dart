import 'dart:io';

import 'package:flutter/services.dart';

import 'disk_space_2_platform_interface.dart';

/// `DiskSpace` is a class that provides methods to get information about the device's disk space.
class DiskSpace {
  static const MethodChannel _channel = MethodChannel('disk_space_2');

  Future<String?> getPlatformVersion() {
    return DiskSpace_2Platform.instance.getPlatformVersion();
  }

  /// Returns the amount of free disk space on the device measured in mebibytes (2^20 bytes).
  ///
  /// This is a static method and can be called directly on the `DiskSpace` class.
  static Future<double?> get getFreeDiskSpace async {
    final double? freeDiskSpace = await _channel.invokeMethod('getFreeDiskSpace');
    return freeDiskSpace;
  }

  /// Returns the total amount of disk space on the device measured in mebibytes (2^20 bytes).
  ///
  /// This is a static method and can be called directly on the `DiskSpace` class.
  static Future<double?> get getTotalDiskSpace async {
    final double? totalDiskSpace = await _channel.invokeMethod('getTotalDiskSpace');
    return totalDiskSpace;
  }

  /// Returns the amount of free disk space for a specific path measured in mebibytes (2^20 bytes).
  ///
  /// This is a static method and can be called directly on the `DiskSpace` class.
  ///
  /// Throws an `Exception` if the specified path does not exist.
  static Future<double?> getFreeDiskSpaceForPath(String path) async {
    if (!Directory(path).existsSync()) {
      throw Exception("Specified path does not exist");
    }
    final double? freeDiskSpace = await _channel.invokeMethod('getFreeDiskSpaceForPath', {"path": path});
    return freeDiskSpace;
  }
}
