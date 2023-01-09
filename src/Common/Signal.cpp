#include "Signal.h"
#include "Logging.h"
#include <fmt/core.h>
#include <unordered_map>
#include <vector>

using namespace common;

static std::unordered_map<int, std::vector<std::function<void(const Signal&)>>> g_signal_handlers_by_signum;

static void internal_signal_handler(int signum) {
    LOG_DEBUG("signal_handler({})", signum);
    if (g_signal_handlers_by_signum.contains(signum)) {
        auto signal = Signal::from_signum(signum);
        for (auto& handler : g_signal_handlers_by_signum.at(signum)) {
            handler(signal);
        }
    }
}

auto common::register_signal_handler(const SignalHander& handler, const std::vector<int>& signal_numbers) -> void {
    for (auto signal_number : signal_numbers) {
        g_signal_handlers_by_signum[signal_number].push_back(handler);
        // FIXME we re-register the global signal handler even when we have
        //  previously registered it
        if (std::signal(signal_number, internal_signal_handler) == SIG_ERR) {
            auto error = Error::from_errno(errno, "signal()", ErrorDomain::CORE);
            LOG_ERROR("Registering signal handler failed: {}", error.error_message());
        }
    }
}

auto Signal::from_signum(int signum) -> Signal {
    return Signal(signum);
}

auto Signal::name() const -> std::string {
    auto* ptr = sigdescr_np(number_);
    return {ptr};
}

auto Signal::to_string() const -> std::string {
    return fmt::format("{}({})", name(), number_);
}
