//
// Created by koushiro on 9/3/18.
//

#include <chrono>
#include <thread>
using namespace std::chrono_literals;

#include "gtest/gtest.h"

#include "tento/base/Timestamp.hpp"
#include "tento/base/Duration.hpp"
using namespace tento;

class TimestampTest : public testing::Test {

};

TEST_F(TimestampTest, IsValid) {
    ASSERT_FALSE(Timestamp::Invalid().IsValid());
    ASSERT_TRUE(Timestamp::Now().IsValid());
}

TEST_F(TimestampTest, DurationSince) {
    auto before = Timestamp::Now();
    std::this_thread::sleep_for(1ms);
    EXPECT_GE(Timestamp::Now().DurationSince(before), Duration::FromMillis(1));
}

TEST_F(TimestampTest, Elapsed) {
    auto time = Timestamp::Now();
    std::this_thread::sleep_for(1ms);
    EXPECT_GE(time.Elapsed(), Duration::FromMillis(1));
}

TEST_F(TimestampTest, TimestampCompute) {
    auto time = Timestamp::Now() + Duration::FromMillis(5);
    std::this_thread::sleep_for(1ms);
    EXPECT_LT(Timestamp::Now(), time);
}