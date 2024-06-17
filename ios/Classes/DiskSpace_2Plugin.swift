import Flutter
import UIKit

public class DiskSpace_2Plugin: NSObject, FlutterPlugin {
   public static func register(with registrar: FlutterPluginRegistrar) {
    let channel = FlutterMethodChannel(name: "disk_space", binaryMessenger: registrar.messenger())
    let instance = DiskSpacePlugin()
    registrar.addMethodCallDelegate(instance, channel: channel)
  }

  public func handle(_ call: FlutterMethodCall, result: @escaping FlutterResult) {
    switch call.method {
    case "getPlatformVersion":
      result("iOS " + UIDevice.current.systemVersion)
    case "getFreeDiskSpace":
        result(UIDevice.current.freeDiskSpaceInMB)
    case "getTotalDiskSpace":
        result(UIDevice.current.totalDiskSpaceInMB)
    case "getFreeDiskSpaceForPath":
        if let arguments = call.arguments as? [String: String], let path = arguments["path"] {
          do {
            let freeDiskSpace = try UIDevice.current.freeDiskSpaceForPathInMB(path: path)
            result(freeDiskSpace)
          } catch {
            result(FlutterError(code: "DISK_SPACE_ERROR", message: error.localizedDescription, details: nil))
          }
        } else {
          result(FlutterError(code: "INVALID_ARGUMENTS", message: "Path is required", details: nil))
        }
    default:
      result(FlutterMethodNotImplemented)
    }
  }
}

extension UIDevice {
    var totalDiskSpaceInMB: Double {
      return Double(totalDiskSpaceInBytes / (1024 * 1024))
    }

    var freeDiskSpaceInMB: Double {
      return Double(freeDiskSpaceInBytes / (1024 * 1024))
    }

    var usedDiskSpaceInMB: Double {
      return Double(usedDiskSpaceInBytes / (1024 * 1024))
    }

    public func freeDiskSpaceForPathInMB(path: String) -> Double {
      return Double(freeDiskSpaceForPathInBytes(path: path) / (1024 * 1024))
    }

    var totalDiskSpaceInBytes:Int64 {
        guard let systemAttributes = try? FileManager.default.attributesOfFileSystem(forPath: NSHomeDirectory() as String),
            let space = (systemAttributes[FileAttributeKey.systemSize] as? NSNumber)?.int64Value else { return 0 }
        return space
    }

    var freeDiskSpaceInBytes: Int64 {
        if let space = try? URL(fileURLWithPath: NSHomeDirectory() as String).resourceValues(forKeys: [URLResourceKey.volumeAvailableCapacityForImportantUsageKey]).volumeAvailableCapacityForImportantUsage {
            return space ?? 0
        } else {
            return 0
        }
    }

    var usedDiskSpaceInBytes:Int64 {
        return totalDiskSpaceInBytes - freeDiskSpaceInBytes
    }

    public func freeDiskSpaceForPathInBytes(path: String) -> Int64 {
        if let space = try? URL(fileURLWithPath: path).resourceValues(forKeys: [URLResourceKey.volumeAvailableCapacityForImportantUsageKey]).volumeAvailableCapacityForImportantUsage {
            return space ?? 0
        } else {
            return 0
        }
    }
}
