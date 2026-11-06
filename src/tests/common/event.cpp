// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>

#include <catch2/catch_test_macros.hpp>

#include "common/thread.h"

#ifndef _WIN32
TEST_CASE("Event extreme timeout waits for and consumes a signal", "[common][event]") {
    Common::Event event;
    std::jthread setter([&] {
        std::mutex mutex;
        std::unique_lock lock{mutex};
        std::condition_variable timer;
        timer.wait_for(lock, std::chrono::milliseconds{20});
        event.Set();
    });
    const bool signalled = event.WaitFor(std::chrono::nanoseconds::max());
    setter.join();
    CHECK(signalled);
    CHECK_FALSE(event.IsSet());
}
#endif

TEST_CASE("Event nonpositive timeout still consumes an existing signal", "[common][event]") {
    Common::Event event;
    CHECK_FALSE(event.WaitFor(std::chrono::nanoseconds::min()));
    CHECK_FALSE(event.WaitFor(std::chrono::nanoseconds::zero()));
    event.Set();
    CHECK(event.WaitFor(std::chrono::nanoseconds::min()));
    CHECK_FALSE(event.IsSet());
    event.Set();
    CHECK(event.WaitFor(std::chrono::nanoseconds::zero()));
    CHECK_FALSE(event.IsSet());
}

TEST_CASE("Event finite timeout does not complete before its deadline", "[common][event]") {
    Common::Event event;
    const auto start = std::chrono::steady_clock::now();
    CHECK_FALSE(event.WaitFor(std::chrono::milliseconds{2}));
    CHECK(std::chrono::steady_clock::now() - start >= std::chrono::milliseconds{2});
}
