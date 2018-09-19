//
// Created by koushiro on 9/18/18.
//

#include "gtest/gtest.h"

#include "tento/base/Thread.hpp"
using namespace tento;

class ThreadTest : public testing::Test {
protected:
    size_t main_tid = thread_id();
    size_t other_tid;
};

TEST_F(ThreadTest, ThreadId) {
    auto thread = Thread([&]() {
       other_tid = thread_id(); 
    });
    EXPECT_NE(main_tid, other_tid);
}