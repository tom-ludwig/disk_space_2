import 'package:plugin_platform_interface/plugin_platform_interface.dart';

import 'disk_space_2_method_channel.dart';

abstract class DiskSpace_2Platform extends PlatformInterface {
  /// Constructs a DiskSpace_2Platform.
  DiskSpace_2Platform() : super(token: _token);

  static final Object _token = Object();

  static DiskSpace_2Platform _instance = MethodChannelDiskSpace_2();

  /// The default instance of [DiskSpace_2Platform] to use.
  ///
  /// Defaults to [MethodChannelDiskSpace_2].
  static DiskSpace_2Platform get instance => _instance;

  /// Platform-specific implementations should set this with their own
  /// platform-specific class that extends [DiskSpace_2Platform] when
  /// they register themselves.
  static set instance(DiskSpace_2Platform instance) {
    PlatformInterface.verifyToken(instance, _token);
    _instance = instance;
  }

  Future<String?> getPlatformVersion() {
    throw UnimplementedError('platformVersion() has not been implemented.');
  }
}
