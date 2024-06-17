import 'package:flutter_test/flutter_test.dart';
import 'package:disk_space_2/disk_space_2.dart';
import 'package:disk_space_2/disk_space_2_platform_interface.dart';
import 'package:disk_space_2/disk_space_2_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

class MockDiskSpace_2Platform with MockPlatformInterfaceMixin implements DiskSpace_2Platform {
  @override
  Future<String?> getPlatformVersion() => Future.value('42');
}

void main() {
  final DiskSpace_2Platform initialPlatform = DiskSpace_2Platform.instance;

  test('$MethodChannelDiskSpace_2 is the default instance', () {
    expect(initialPlatform, isInstanceOf<MethodChannelDiskSpace_2>());
  });

  test('getPlatformVersion', () async {
    DiskSpace diskSpace_2Plugin = DiskSpace();
    MockDiskSpace_2Platform fakePlatform = MockDiskSpace_2Platform();
    DiskSpace_2Platform.instance = fakePlatform;

    expect(await diskSpace_2Plugin.getPlatformVersion(), '42');
  });
}
