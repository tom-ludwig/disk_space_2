package com.example.disk_space_2

import android.os.Environment
import android.os.StatFs
import android.os.Handler
import android.os.Looper
import io.flutter.embedding.engine.plugins.FlutterPlugin
import io.flutter.plugin.common.MethodCall
import io.flutter.plugin.common.MethodChannel
import io.flutter.plugin.common.MethodChannel.MethodCallHandler
import io.flutter.plugin.common.MethodChannel.Result
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

/** DiskSpace_2Plugin */
class DiskSpace_2Plugin : FlutterPlugin, MethodCallHandler {
  /// The MethodChannel that will the communication between Flutter and native Android
  ///
  /// This local reference serves to register the plugin with the Flutter Engine and unregister it
  /// when the Flutter Engine is detached from the Activity
  private lateinit var channel: MethodChannel
  private val executor: ExecutorService = Executors.newSingleThreadExecutor()
  private val mainHandler = Handler(Looper.getMainLooper())

  override fun onAttachedToEngine(flutterPluginBinding: FlutterPlugin.FlutterPluginBinding) {
    channel = MethodChannel(flutterPluginBinding.binaryMessenger, "disk_space_2")
    channel.setMethodCallHandler(this)
  }

  override fun onMethodCall(call: MethodCall, result: Result) {
    when (call.method) {
      "getFreeDiskSpace" -> {
        executeAsync(result, "Failed to get free disk space") {
          getFreeDiskSpace()
        }
      }
      "getTotalDiskSpace" -> {
        executeAsync(result, "Failed to get total disk space") {
          getTotalDiskSpace()
        }
      }
      "getFreeDiskSpaceForPath" -> {
        val path = call.argument<String>("path")
        if (path != null) {
          executeAsync(result, "Failed to get free disk space for path") {
            getFreeDiskSpaceForPath(path)
          }
        } else {
          result.error("INVALID_ARGUMENT", "Path is required", null)
        }
      }
      "getPlatformVersion" -> result.success("Android ${android.os.Build.VERSION.RELEASE}")
      else -> result.notImplemented()
    }
  }

  override fun onDetachedFromEngine(binding: FlutterPlugin.FlutterPluginBinding) {
    executor.shutdown()
    channel.setMethodCallHandler(null)
  }

  private fun executeAsync(result: Result, errorMessage: String, operation: () -> Double) {
    executor.execute {
      try {
        val value = operation()
        mainHandler.post {
          result.success(value)
        }
      } catch (e: Exception) {
        mainHandler.post {
          result.error("DISK_SPACE_ERROR", "$errorMessage: ${e.message}", null)
        }
      }
    }
  }

  private fun getFreeDiskSpace(): Double {
    val stat = StatFs(Environment.getExternalStorageDirectory().path)

    val bytesAvailable: Long
    bytesAvailable =
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN_MR2)
            stat.blockSizeLong * stat.availableBlocksLong
        else stat.blockSize.toLong() * stat.availableBlocks.toLong()
    return (bytesAvailable / (1024f * 1024f)).toDouble()
  }

  private fun getFreeDiskSpaceForPath(path: String): Double {
    val stat = StatFs(path)

    val bytesAvailable: Long
    bytesAvailable =
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN_MR2)
            stat.blockSizeLong * stat.availableBlocksLong
        else stat.blockSize.toLong() * stat.availableBlocks.toLong()
    return (bytesAvailable / (1024f * 1024f)).toDouble()
  }

  private fun getTotalDiskSpace(): Double {
    val stat = StatFs(Environment.getExternalStorageDirectory().path)

    val bytesAvailable: Long
    bytesAvailable =
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN_MR2)
            stat.blockSizeLong * stat.blockCountLong
        else stat.blockSize.toLong() * stat.blockCount.toLong()
    return (bytesAvailable / (1024f * 1024f)).toDouble()
  }
}
