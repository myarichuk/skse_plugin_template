#pragma once

#include <Windows.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

using namespace spdlog;
namespace logger = SKSE::log;

inline void SetupLog() {
    const auto logs_folder = SKSE::log::log_directory();

    if (!logs_folder) 
        SKSE::stl::report_and_fail("SKSE log_directory not provided, logs disabled.");

    auto plugin_name = SKSE::PluginDeclaration::GetSingleton()->GetName();
    const auto log_file_path = *logs_folder / std::format("{}.log", plugin_name);
    auto file_logger_ptr = std::make_shared<sinks::basic_file_sink_mt>(log_file_path.string(), true);
    std::shared_ptr<spdlog::logger> loggerPtr;

    if (IsDebuggerPresent()) {
        auto debug_logger_ptr = std::make_shared<sinks::msvc_sink_mt>();
        sinks_init_list loggers{std::move(file_logger_ptr), std::move(debug_logger_ptr)};
        loggerPtr = std::make_shared<spdlog::logger>("log", loggers);
    } else {
        // If no debugger is attached, only log to the file.
        loggerPtr = std::make_shared<spdlog::logger>("log", std::move(file_logger_ptr));
    }
    set_default_logger(std::move(loggerPtr));

    set_level(level::trace);
    flush_on(level::trace);
}