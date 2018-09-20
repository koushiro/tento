//
// Created by koushiro on 9/18/18.
//

#include "gtest/gtest.h"

#include "tento/base/CountDownLatch.hpp"
#include "tento/base/Thread.hpp"
using namespace tento;

class ThreadTest : public testing::Test {
protected:
    size_t main_tid = thread_id();
    size_t other_tid = thread_id();
};

TEST_F(ThreadTest, ThreadId) {
    CountDownLatch latch{1};
    auto thread = Thread([&]() {
       other_tid = thread_id();
       latch.CountDown();
    });
    latch.Wait();
    EXPECT_NE(main_tid, other_tid);
}