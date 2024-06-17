#ifndef _LOGGING_H
#define _LOGGING_H

#define LOG_BUF_SIZE 8196

// #include <android/log.h>
#include <format>
#include <array>

#define ANDROID_LOG_UNKNOWN 0
#define ANDROID_LOG_DEFAULT 1
#define ANDROID_LOG_VERBOSE 2
#define ANDROID_LOG_DEBUG 3
#define ANDROID_LOG_INFO 4
#define ANDROID_LOG_WARN 5
#define ANDROID_LOG_ERROR 6
#define ANDROID_LOG_FATAL 7
#define ANDROID_LOG_SILENT 8

#ifndef LOG_TAG
#define LOG_TAG    "XMONO"
#endif
extern "C" int __android_log_print(int prio, const char *tag,  const char *fmt,...);

#ifdef LOG_DISABLED
#define LOGD(...) 0
#define LOGV(...) 0
#define LOGI(...) 0
#define LOGW(...) 0
#define LOGE(...) 0
#else
template <typename... T>
constexpr inline void LOG(int prio, const char* tag, std::format_string<T...> fmt, T&&... args) {
    std::array<char, LOG_BUF_SIZE> buf{};
    auto s = std::format_to_n(buf.data(), buf.size(), fmt, std::forward<T>(args)...).size;
    buf[s] = '\0';
    __android_log_print(prio, tag, buf.data());
}
#ifndef NDEBUG
#define LOGD(fmt, ...) LOG(ANDROID_LOG_DEBUG, LOG_TAG, "{}:{}#{}" ": " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__ __VA_OPT__(,) __VA_ARGS__)
#define LOGV(fmt, ...) LOG(ANDROID_LOG_VERBOSE, LOG_TAG, "{}:{}#{}" ": " fmt, __FILE_NAME__, __LINE__, __PRETTY_FUNCTION__ __VA_OPT__(,) __VA_ARGS__)
#else
#define LOGD(...) 0
#define LOGV(...) 0
#endif
#define LOGI(...)  LOG(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...)  LOG(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  LOG(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGF(...)  LOG(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)
#define PLOGE(fmt, args...) LOGE(fmt " failed with {}: {}", ##args, errno, strerror(errno))
#endif

#endif // _LOGGING_H