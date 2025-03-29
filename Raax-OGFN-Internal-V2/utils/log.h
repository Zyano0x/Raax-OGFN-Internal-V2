#pragma once
#include <globals.h>
#include <string>
#include <extern/loguru/loguru.hpp>

#define LOG_ERROR loguru::Verbosity_ERROR
#define LOG_WARN loguru::Verbosity_WARNING
#define LOG_INFO loguru::Verbosity_INFO
#define LOG_TRACE loguru::Verbosity_1

#if CFG_USELOGGING
void InitLogger();
void SetThreadLogName(const std::string& Name);
#define LOG(LogLevel, Message, ...) VLOG_F(LogLevel, Message, ##__VA_ARGS__)
#else
#define LOG void(0)
#endif

#if CFG_SHOWCONSOLE
void CreateConsole();
#endif
