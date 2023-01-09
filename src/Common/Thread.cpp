#include "Thread.h"
#include "Logging.h"
#include <fmt/core.h>
#include <iosfwd>

using namespace common;

Thread::Thread(std::unique_ptr<Runnable>&& runnable) :
    runnable_(std::move(runnable)) {}

Thread::~Thread() noexcept {
    stop();
}

auto Thread::to_string() const -> std::string {
    std::ostringstream ss;
    ss << thread_.get_id();
    return fmt::format("Thread{{id={}}}", ss.str());
}

auto Thread::start() -> void {
    thread_ = std::jthread(&Runnable::run, runnable_.get());
    LOG_DEBUG("{}::start()", to_string());
}

auto Thread::stop() noexcept -> void {
    LOG_DEBUG("{}::stop()", to_string());
    try {
        (*runnable_).request_stop();
        join();
    } catch (const std::exception& e) {
        LOG_ERROR("{}::join() failed: {}", to_string(), e.what());
    }
}

auto Thread::join() -> void {
    thread_.join();
}
