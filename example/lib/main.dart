import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:disk_space_2/disk_space_2.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  String _platformVersion = 'Unknown';
  double _totalDiskSpace = 0.0;
  double _freeDiskSpace = 0.0;
  // double _freeDiskSpaceForPath = 0.0;

  final _diskSpacePlugin = DiskSpace();

  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  // Platform messages are asynchronous, so we initialize in an async method.
  Future<void> initPlatformState() async {
    String platformVersion;
    double totalDiskSpace;
    double freeDiskSpace;
    // double freeDiskSpaceForPath;

    // Platform messages may fail, so we use a try/catch PlatformException.
    // We also handle the message potentially returning null.
    try {
      platformVersion = await _diskSpacePlugin.getPlatformVersion() ?? 'Unknown platform version';
    } on PlatformException {
      platformVersion = 'Failed to get platform version.';
    }

    totalDiskSpace = await DiskSpace.getTotalDiskSpace ?? 0.0;
    freeDiskSpace = await DiskSpace.getFreeDiskSpace ?? 0.0;

    if (!mounted) return;

    setState(() {
      _platformVersion = platformVersion;
      _totalDiskSpace = totalDiskSpace;
      _freeDiskSpace = freeDiskSpace;
    });
  }

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Column(
            children: [
              Text('Running on: $_platformVersion'),
              Text('Total disk space: $_totalDiskSpace mb'),
              Text('Free disk space: $_freeDiskSpace mb'),
              Text('Free disk space: ${_freeDiskSpace / 1000} GB'),
            ],
          ),
        ),
      ),
    );
  }
}
