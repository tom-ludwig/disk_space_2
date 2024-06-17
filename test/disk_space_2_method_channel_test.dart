import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:disk_space_2/disk_space_2_method_channel.dart';

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();

  MethodChannelDiskSpace_2 platform = MethodChannelDiskSpace_2();
  const MethodChannel channel = MethodChannel('disk_space_2');

  setUp(() {
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger.setMockMethodCallHandler(
      channel,
      (MethodCall methodCall) async {
        return '42';
      },
    );
  });

  tearDown(() {
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger.setMockMethodCallHandler(channel, null);
  });

  test('getPlatformVersion', () async {
    expect(await platform.getPlatformVersion(), '42');
  });
}
