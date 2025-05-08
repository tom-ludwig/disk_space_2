#include "include/disk_space_2/disk_space_2_plugin.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>

#include <sys/utsname.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <errno.h>
#include <string.h>

#include <cstring>

#define DISK_SPACE_2_PLUGIN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), disk_space_2_plugin_get_type(), \
                              DiskSpace_2Plugin))

struct _DiskSpace_2Plugin {
  GObject parent_instance;
};

G_DEFINE_TYPE(DiskSpace_2Plugin, disk_space_2_plugin, g_object_get_type())

constexpr char kBadArgumentsError[] = "Bad Arguments";
constexpr char kNoMemory[] = "Out of memory";
constexpr char kIOError[] = "IO Error";
constexpr char PATH_KEY[] = "path";
constexpr double ONE_MEGABYTE = 1024 * 1024;
constexpr size_t MAX_PATH_LENGTH = 4096;

enum class WhichMethod {
  UNKNOWN,
  FREE_SPACE_PATH,
  TOTAL_SPACE_PATH,
  FREE_SPACE,
  TOTAL_SPACE,
  PLATFORM_VER,
};

// Helper function to get disk space information
static bool get_disk_space(const char* path, struct statvfs* fs_info, GError** error) {
  if (strlen(path) > MAX_PATH_LENGTH) {
    g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_INVAL,
                "Path length exceeds maximum allowed length");
    return false;
  }

  if (statvfs(path, fs_info) != 0) {
    g_set_error(error, G_FILE_ERROR, g_file_error_from_errno(errno),
                "Failed to get disk space info: %s", strerror(errno));
    return false;
  }
  return true;
}

// Called when a method call is received from Flutter.
static void disk_space_2_plugin_handle_method_call(
    DiskSpace_2Plugin* self,
    FlMethodCall* method_call) {
  g_autoptr(FlMethodResponse) response = nullptr;
  GError* error = nullptr;

  const gchar* method = fl_method_call_get_name(method_call);
  FlValue* args = fl_method_call_get_args(method_call);

  auto whichMethod = WhichMethod::UNKNOWN;
  if (strcmp(method, "getFreeDiskSpaceForPath") == 0) {
    whichMethod = WhichMethod::FREE_SPACE_PATH;
  } else if (strcmp(method, "getTotalDiskSpaceForPath") == 0) {
    whichMethod = WhichMethod::TOTAL_SPACE_PATH;
  } else if (strcmp(method, "getFreeDiskSpace") == 0) {
    whichMethod = WhichMethod::FREE_SPACE;
  } else if (strcmp(method, "getTotalDiskSpace") == 0) {
    whichMethod = WhichMethod::TOTAL_SPACE;
  } else if (strcmp(method, "getPlatformVersion") == 0) {
    whichMethod = WhichMethod::PLATFORM_VER;
  }

  switch (whichMethod) {
    case WhichMethod::FREE_SPACE_PATH:
    case WhichMethod::TOTAL_SPACE_PATH: {
      if ((args == nullptr) || (fl_value_get_type(args) != FL_VALUE_TYPE_MAP)) {
        response = FL_METHOD_RESPONSE(fl_method_error_response_new(
            kBadArgumentsError, "Expected Map", nullptr));
        break;
      }
      FlValue* pathValue = fl_value_lookup_string(args, PATH_KEY);
      if ((pathValue == nullptr) || (fl_value_get_type(pathValue) != FL_VALUE_TYPE_STRING)) {
        response = FL_METHOD_RESPONSE(fl_method_error_response_new(
            kBadArgumentsError, "Expected string in Map entry", nullptr));
        break;
      }
      const gchar* path = fl_value_get_string(pathValue);
      struct statvfs fs_info;
      if (!get_disk_space(path, &fs_info, &error)) {
        response = FL_METHOD_RESPONSE(fl_method_error_response_new(
            kIOError, error->message, nullptr));
        g_error_free(error);
        break;
      }

      double space = whichMethod == WhichMethod::FREE_SPACE_PATH ?
          (fs_info.f_bsize * fs_info.f_bavail / ONE_MEGABYTE) :
          (fs_info.f_frsize * fs_info.f_blocks / ONE_MEGABYTE);
      
      g_autoptr(FlValue) result = fl_value_new_float(space);
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
      break;
    }
    case WhichMethod::FREE_SPACE:
    case WhichMethod::TOTAL_SPACE: {
      struct passwd pwd = {};
      struct passwd* pwdResult = nullptr;
      size_t pathBufferSize = sysconf(_SC_GETPW_R_SIZE_MAX);
      if (pathBufferSize == static_cast<size_t>(-1))
        pathBufferSize = 16384;
      
      g_autofree char* pathBuffer = static_cast<char*>(g_malloc(pathBufferSize));
      if (pathBuffer == nullptr) {
        response = FL_METHOD_RESPONSE(fl_method_error_response_new(
            kNoMemory, "Out of memory querying pwd entry size", nullptr));
        break;
      }

      if (getpwuid_r(getuid(), &pwd, pathBuffer, pathBufferSize, &pwdResult) != 0) {
        response = FL_METHOD_RESPONSE(fl_method_error_response_new(
            kNoMemory, "Failed to get pwd entry", nullptr));
        break;
      }

      struct statvfs fs_info;
      if (!get_disk_space(pwd.pw_dir, &fs_info, &error)) {
        response = FL_METHOD_RESPONSE(fl_method_error_response_new(
            kIOError, error->message, nullptr));
        g_error_free(error);
        break;
      }

      double space = whichMethod == WhichMethod::FREE_SPACE ?
          (fs_info.f_bsize * fs_info.f_bavail / ONE_MEGABYTE) :
          (fs_info.f_frsize * fs_info.f_blocks / ONE_MEGABYTE);
      
      g_autoptr(FlValue) result = fl_value_new_float(space);
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
      break;
    }
    case WhichMethod::PLATFORM_VER: {
      struct utsname uname_data = {};
      if (uname(&uname_data) != 0) {
        response = FL_METHOD_RESPONSE(fl_method_error_response_new(
            kIOError, "Failed to get system information", nullptr));
        break;
      }
      g_autofree gchar* version = g_strdup_printf("Linux %s", uname_data.version);
      g_autoptr(FlValue) result = fl_value_new_string(version);
      response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
      break;
    }
    default: {
      response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
      break;
    }
  }
  fl_method_call_respond(method_call, response, nullptr);
}

static void disk_space_2_plugin_dispose(GObject* object) {
  G_OBJECT_CLASS(disk_space_2_plugin_parent_class)->dispose(object);
}

static void disk_space_2_plugin_class_init(DiskSpace_2PluginClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = disk_space_2_plugin_dispose;
}

static void disk_space_2_plugin_init(DiskSpace_2Plugin* self) {}

static void method_call_cb(FlMethodChannel* channel, FlMethodCall* method_call,
                           gpointer user_data) {
  DiskSpace_2Plugin* plugin = DISK_SPACE_2_PLUGIN(user_data);
  disk_space_2_plugin_handle_method_call(plugin, method_call);
}

void disk_space_2_plugin_register_with_registrar(FlPluginRegistrar* registrar) {
  DiskSpace_2Plugin* plugin = DISK_SPACE_2_PLUGIN(
      g_object_new(disk_space_2_plugin_get_type(), nullptr));

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            "disk_space_2",
                            FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(channel, method_call_cb,
                                            g_object_ref(plugin),
                                            g_object_unref);

  g_object_unref(plugin);
}
