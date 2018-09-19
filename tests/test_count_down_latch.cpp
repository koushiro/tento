//
// Created by koushiro on 9/3/18.
//

#include "gtest/gtest.h"

#include "tento/base/CountDownLatch.hpp"
#include "tento/base/Thread.hpp"
using namespace tento;

class CountDownLatchTest : public testing::Test {
protected:
    CountDownLatch latch_{3};
};

TEST_F(CountDownLatchTest, CountDownAndGetCount) {
    EXPECT_EQ(latch_.GetCount(), 3);

    Thread thread(
        [&]() {
            auto count = latch_.GetCount();
            while (count > 0) {
                latch_.CountDown();
                EXPECT_EQ(count, (latch_.GetCount() + 1));
                count = latch_.GetCount();
            }
        }
    );

    latch_.Wait();
    EXPECT_EQ(latch_.GetCount(), 0);
}