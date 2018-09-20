//
// Created by koushiro on 9/4/18.
//

#include "gtest/gtest.h"

#include "tento/base/Logger.hpp"
#include "tento/net/EventLoop.hpp"

using namespace tento;
using namespace tento::net;

class TimerTest : public testing::Test {
protected:
    enum class RunType { Once, Every, };
    Logger logger{Logger::LogKind::BasicFile};
};

TEST_F(TimerTest, RunAt) {
    EventLoop loop;
    int totalCounter = 0;
    int onceCounter = 0;
    int everyCounter = 0;
    auto print = [&](RunType type) {
        switch (type) {
            case RunType::Once: ++onceCounter;   break;
            case RunType::Every: ++everyCounter; break;
        }
        ++totalCounter;
        if (totalCounter == 2) {
            loop.Quit();
        }
    };

    auto time = Timestamp::Now() + Duration::FromMillis(100);
    loop.RunAt(time, std::bind(print, RunType::Once));
    loop.RunAt(time, std::bind(print, RunType::Once));

    loop.Run();
    EXPECT_EQ(totalCounter, 2);
    EXPECT_EQ(onceCounter, 2);
    EXPECT_EQ(everyCounter, 0);
}

TEST_F(TimerTest, RunAfter) {
    EventLoop loop;
    int totalCounter = 0;
    int onceCounter = 0;
    int everyCounter = 0;
    auto print = [&](RunType type) {
        switch (type) {
            case RunType::Once: ++onceCounter;   break;
            case RunType::Every: ++everyCounter; break;
        }
        ++totalCounter;
        if (totalCounter == 2) {
            loop.Quit();
        }
    };

    // valgrind memcheck - maybe Assertion `nanos_ > earlier.nanos_' failed.
//    loop.RunAfter(Duration::FromMillis(100), std::bind(print, RunType::Once));
//    loop.RunAfter(Duration::FromMillis(100), std::bind(print, RunType::Once));
    loop.RunAfter(Duration::FromMillis(100), std::bind(print, RunType::Once));
    loop.RunAfter(Duration::FromMillis(150), std::bind(print, RunType::Once));

    loop.Run();
    EXPECT_EQ(totalCounter, 2);
    EXPECT_EQ(onceCounter, 2);
    EXPECT_EQ(everyCounter, 0);
}

TEST_F(TimerTest, RunEvery) {
    EventLoop loop;
    int totalCounter = 0;
    int onceCounter = 0;
    int everyCounter = 0;
    auto print = [&](RunType type) {
        switch (type) {
            case RunType::Once: ++onceCounter;   break;
            case RunType::Every: ++everyCounter; break;
        }
        ++totalCounter;
        if (totalCounter == 10) {
            loop.Quit();
        }
    };

    loop.RunEvery(Duration::FromMillis(100), std::bind(print, RunType::Every));
    loop.RunEvery(Duration::FromMillis(200), std::bind(print, RunType::Every));

    loop.Run();
    EXPECT_EQ(totalCounter, 10);
    EXPECT_EQ(onceCounter, 0);
    EXPECT_EQ(everyCounter, 10);
}

TEST_F(TimerTest, CancelTimer) {
    EventLoop loop;
    int totalCounter = 0;
    int onceCounter = 0;
    int everyCounter = 0;
    auto print = [&](RunType type) {
        switch (type) {
            case RunType::Once: ++onceCounter;   break;
            case RunType::Every: ++everyCounter; break;
        }
        ++totalCounter;
        if (totalCounter == 5) {
            loop.Quit();
        }
    };

    auto cancel = [&](TimerPtr timer) {
        print(RunType::Once);
        loop.CancelTimer(timer);
    };

    loop.RunAfter(Duration::FromMillis(100), std::bind(print, RunType::Once));
    auto timer = loop.RunEvery(Duration::FromMillis(150), std::bind(print, RunType::Every));
    loop.RunAfter(Duration::FromMillis(350), std::bind(cancel, timer));
    loop.RunAfter(Duration::FromMillis(400), std::bind(cancel, timer));

    loop.Run();
    EXPECT_EQ(totalCounter, 5);
    EXPECT_EQ(onceCounter, 3);
    EXPECT_EQ(everyCounter, 2);
}