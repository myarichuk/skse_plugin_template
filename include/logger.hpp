#pragma once

#include <Windows.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

using namespace spdlog;
namespace logger = SKSE::log;

inline void SetupLog() {
    const auto logsFolder = SKSE::log::log_directory();

    if (!logsFolder) 
        SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");

    auto pluginName = SKSE::PluginDeclaration::GetSingleton()->GetName();
    const auto logFilePath = *logsFolder / std::format("{}.log", pluginName);
    auto fileLoggerPtr = std::make_shared<sinks::basic_file_sink_mt>(logFilePath.string(), true);
    std::shared_ptr<spdlog::logger> loggerPtr;

    if (IsDebuggerPresent()) {
        auto debugLoggerPtr = std::make_shared<sinks::msvc_sink_mt>();
        sinks_init_list loggers{std::move(fileLoggerPtr), std::move(debugLoggerPtr)};
        loggerPtr = std::make_shared<spdlog::logger>("log", loggers);
    } else {
        // If no debugger is attached, only log to the file.
        loggerPtr = std::make_shared<spdlog::logger>("log", std::move(fileLoggerPtr));
    }
    set_default_logger(std::move(loggerPtr));

    set_level(level::trace);
    flush_on(level::trace);
}