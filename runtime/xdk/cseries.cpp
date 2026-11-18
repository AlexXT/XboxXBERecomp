// The debug build's assertions: display_assert shows the blue screen and the
// caller's system_exit(-1) stops the title. A test map can trip a check that the
// release build does not have (Halo 2's earthcity hides an already hidden item
// when its test squad cannot find a seat). With XBE_SKIP_ASSERTS=1 the check is
// logged and passed over, which is what the release build does; without it the
// title behaves exactly as before.
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

#include "core/log.h"
#include "xdk/hle.h"

using namespace kernel;

namespace {

bool SkipAsserts() {
    static const bool on = [] {
        const char* e = std::getenv("XBE_SKIP_ASSERTS");
        bool v = e && *e && *e != '0';
        if (v) LOGR("cseries: XBE_SKIP_ASSERTS=1, assertions are logged and passed over");
        return v;
    }();
    return on;
}

const char* GuestString(uint32_t va) {
    if (!va) return "(null)";
    const char* s = static_cast<const char*>(HostPtr(va));
    return s ? s : "(bad)";
}

// A broken test map can fail the same check every frame; logging every one of
// them (two synchronous writes per assert) drops the title to a few FPS. Log
// each distinct message at most once every two seconds, with a repeat count.
bool LogAssertion(const std::string& key, unsigned long long& count) {
    struct Stats {
        std::mutex mu;
        std::unordered_map<std::string, std::pair<unsigned long long, double>> seen;
        static double Now() {
            using clock = std::chrono::steady_clock;
            static const auto start = clock::now();
            return std::chrono::duration<double>(clock::now() - start).count();
        }
    };
    static Stats stats;
    std::lock_guard<std::mutex> lock(stats.mu);
    auto& entry = stats.seen[key];
    count = ++entry.first;
    const double now = Stats::Now();
    if (now - entry.second < 2.0) return false;
    entry.second = now;
    return true;
}

}  // namespace

// (const char* text, const char* file, long line, bool fatal)
X86_EXTERN_FUNC(GUEST_ORIG(display_assert));
X86_FUNC(GUEST(display_assert)) {
    if (SkipAsserts()) {
        const std::string key = std::string(GuestString(R32(ctx.esp + 4))) + " at " +
                                GuestString(R32(ctx.esp + 8)) + ",#" +
                                std::to_string(int32_t(R32(ctx.esp + 12)));
        unsigned long long count = 0;
        if (LogAssertion(key, count)) LOGR("assert: %s (seen %llu)", key.c_str(), count);
        ctx.esp += 4;  // cdecl: pop the return address, the caller pops the four arguments
        return;
    }
    GUEST_ORIG(display_assert)(ctx, base);
}

// (int code): the assertion macro calls system_exit(-1) when no debugger is attached
X86_EXTERN_FUNC(GUEST_ORIG(system_exit));
X86_FUNC(GUEST(system_exit)) {
    if (SkipAsserts() && int32_t(R32(ctx.esp + 4)) == -1) {
        unsigned long long count = 0;
        if (LogAssertion("system_exit(-1)", count))
            LOGR("cseries: system_exit(-1) after an assertion is passed over (%llu)", count);
        ctx.esp += 4;  // cdecl: pop the return address, the caller pops the argument
        return;
    }
    GUEST_ORIG(system_exit)(ctx, base);
}
