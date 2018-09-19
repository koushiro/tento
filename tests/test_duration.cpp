//
// Created by koushiro on 9/3/18.
//

#include "gtest/gtest.h"

#include "tento/base/Duration.hpp"
using namespace tento;

class DurationTest : public testing::Test {
protected:
    Duration nanos_ = Duration::FromNanos(12'345'000'000);
    Duration micros_ = Duration::FromMicros(12'345'000);
    Duration millis_ = Duration::FromMillis(12'345);
    Duration secs_ = Duration::FromSecs(12'345);
    Duration mins_ = Duration::FromMins(30);
    Duration hours_ = Duration::FromHours(12);
};

TEST_F(DurationTest, IsZero) {
    Duration zero_ = Duration{0, 0};
    ASSERT_TRUE(zero_.IsZero());
}

TEST_F(DurationTest, NanosOperation) {
    EXPECT_EQ(nanos_.SecsPart(), 12);
    EXPECT_EQ(nanos_.SubSecNanos(), 345'000'000);
    EXPECT_EQ(nanos_.SubSecMicros(), 345'000);
    EXPECT_EQ(nanos_.SubSecMillis(), 345);
    EXPECT_EQ(nanos_.AsNanos(), 12'345'000'000);
    EXPECT_EQ(nanos_.AsMicros(), 12'345'000);
    EXPECT_EQ(nanos_.AsMillis(), 12'345);
    EXPECT_EQ(nanos_.AsSecs(), 12.345);
}

TEST_F(DurationTest, MicrosOperation) {
    EXPECT_EQ(micros_.SecsPart(), 12);
    EXPECT_EQ(micros_.SubSecNanos(), 345'000'000);
    EXPECT_EQ(micros_.SubSecMicros(), 345'000);
    EXPECT_EQ(micros_.SubSecMillis(), 345);
    EXPECT_EQ(micros_.AsNanos(), 12'345'000'000);
    EXPECT_EQ(micros_.AsMicros(), 12'345'000);
    EXPECT_EQ(micros_.AsMillis(), 12'345);
    EXPECT_EQ(micros_.AsSecs(), 12.345);
}

TEST_F(DurationTest, MillisOperation) {
    EXPECT_EQ(millis_.SecsPart(), 12);
    EXPECT_EQ(millis_.SubSecNanos(), 345'000'000);
    EXPECT_EQ(millis_.SubSecMicros(), 345'000);
    EXPECT_EQ(millis_.SubSecMillis(), 345);
    EXPECT_EQ(millis_.AsNanos(), 12'345'000'000);
    EXPECT_EQ(millis_.AsMicros(), 12'345'000);
    EXPECT_EQ(millis_.AsMillis(), 12'345);
    EXPECT_EQ(millis_.AsSecs(), 12.345);
}

TEST_F(DurationTest, SecsOperation) {
    EXPECT_EQ(secs_.SecsPart(), 12345);
    EXPECT_EQ(secs_.SubSecNanos(), 0);
    EXPECT_EQ(secs_.SubSecMicros(), 0);
    EXPECT_EQ(secs_.SubSecMillis(), 0);
    EXPECT_EQ(secs_.AsNanos(), 12'345'000'000'000);
    EXPECT_EQ(secs_.AsMicros(), 12'345'000'000);
    EXPECT_EQ(secs_.AsMillis(), 12'345'000);
    EXPECT_EQ(secs_.AsSecs(), 12345.0);
    EXPECT_EQ(secs_.AsMins(), 12'345.0 / 60);
    EXPECT_EQ(secs_.AsHours(), 12'345.0 / 3600);
}

TEST_F(DurationTest, MinsOperation) {
    EXPECT_EQ(mins_.SecsPart(), 1800);
    EXPECT_EQ(mins_.SubSecNanos(), 0);
    EXPECT_EQ(mins_.SubSecMicros(), 0);
    EXPECT_EQ(mins_.SubSecMillis(), 0);
    EXPECT_EQ(mins_.AsNanos(), 1'800'000'000'000);
    EXPECT_EQ(mins_.AsMicros(), 1'800'000'000);
    EXPECT_EQ(mins_.AsMillis(), 1'800'000);
    EXPECT_EQ(mins_.AsSecs(), 1800.0);
    EXPECT_EQ(mins_.AsMins(), 30.0);
    EXPECT_EQ(mins_.AsHours(), 0.5);
}

TEST_F(DurationTest, HoursOperation) {
    EXPECT_EQ(hours_.SecsPart(), 43200);
    EXPECT_EQ(hours_.SubSecNanos(), 0);
    EXPECT_EQ(hours_.SubSecMicros(), 0);
    EXPECT_EQ(hours_.SubSecMillis(), 0);
    EXPECT_EQ(hours_.AsNanos(), 43'200'000'000'000);
    EXPECT_EQ(hours_.AsMicros(), 43'200'000'000);
    EXPECT_EQ(hours_.AsMillis(), 43'200'000);
    EXPECT_EQ(hours_.AsSecs(), 43200.0);
    EXPECT_EQ(hours_.AsMins(), 720.0);
    EXPECT_EQ(hours_.AsHours(), 12.0);
}

TEST_F(DurationTest, DurationCompare) {
    EXPECT_EQ(nanos_, micros_);
    EXPECT_EQ(micros_, millis_);
    EXPECT_EQ(nanos_, millis_);
    EXPECT_GT(secs_, millis_);
    EXPECT_GT(hours_, mins_);
}

TEST_F(DurationTest, DurationCompute) {
    EXPECT_EQ(hours_ + mins_, Duration::FromMins(750));
    EXPECT_EQ(hours_ - mins_, Duration::FromMins(690));
    EXPECT_EQ(mins_ * 2, Duration::FromMins(60));
    EXPECT_EQ(hours_ / 2, Duration::FromHours(6));
}