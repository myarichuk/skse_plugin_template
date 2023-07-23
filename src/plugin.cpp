#include <logger.hpp>
#include <boost/signals2.hpp>


SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    Init(skse);
    
    SetupLog();

    info("[Plugin] Initialized");

    // note: this prints the message in the console, not in the log file
    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message *message) {
        if (message->type == SKSE::MessagingInterface::kDataLoaded) {
            RE::ConsoleLog::GetSingleton()->Print("[Plugin] Initialized");

        }
    });

    return true;
}