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
    Logger logger_ { Logger::LogKind::BasicFile };
};

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
    loop.RunAfter(Duration::FromMillis(100), std::bind(print, RunType::Once));
    loop.RunAfter(Duration::FromMillis(100), std::bind(print, RunType::Once));

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
        if (totalCounter == 5) {
            loop.Quit();
        }
    };

    loop.RunEvery(Duration::FromMillis(100), std::bind(print, RunType::Every));

    loop.Run();
    EXPECT_EQ(totalCounter, 5);
    EXPECT_EQ(onceCounter, 0);
    EXPECT_EQ(everyCounter, 5);
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

    auto cancel = [&](TimerId timerId) {
        print(RunType::Once);
        loop.CancelTimer(timerId);
    };

    loop.RunAfter(Duration::FromMillis(100), std::bind(print, RunType::Once));
    TimerId id = loop.RunEvery(Duration::FromMillis(100), std::bind(print, RunType::Every));
    loop.RunAfter(Duration::FromMillis(250), std::bind(cancel, id));
    loop.RunAfter(Duration::FromMillis(300), std::bind(cancel, id));

    loop.Run();
    EXPECT_EQ(totalCounter, 5);
    EXPECT_EQ(onceCounter, 3);
    EXPECT_EQ(everyCounter, 2);
}