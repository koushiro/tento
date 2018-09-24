//
// Created by koushiro on 9/20/18.
//

#include "gtest/gtest.h"

#include "tento/net/Buffer.hpp"
using namespace tento::net;

class BufferTest : public testing::Test {
protected:
    Buffer buffer;
};

TEST_F(BufferTest, InitializedBuffer) {
    EXPECT_EQ(buffer.PrependableBytes(), Buffer::kCheapPrependSize);
    EXPECT_EQ(buffer.ReadableBytes(), 0);
    EXPECT_EQ(buffer.WritableBytes(), Buffer::kInitialSize);
    EXPECT_EQ(buffer.Capacity(), Buffer::kCheapPrependSize + Buffer::kInitialSize);
    EXPECT_EQ(buffer.Size(), Buffer::kCheapPrependSize + Buffer::kInitialSize);
}

TEST_F(BufferTest, Append) {
    std::string data {"HelloWorld"};
    buffer.Append(data.c_str(), data.size());
    EXPECT_EQ(buffer.PrependableBytes(), Buffer::kCheapPrependSize);
    EXPECT_EQ(buffer.ReadableBytes(), data.size());
    EXPECT_EQ(buffer.WritableBytes(), Buffer::kInitialSize - data.size());
    EXPECT_EQ(buffer.Capacity(), Buffer::kCheapPrependSize + Buffer::kInitialSize);
    EXPECT_EQ(buffer.Size(), Buffer::kCheapPrependSize + Buffer::kInitialSize);
}

TEST_F(BufferTest, AppendManyBytes) {
    std::vector<int16_t> data;
    for (int16_t i = 0; i < 4096; ++i) {
        data.push_back(i);
    }
    buffer.Append(data.data(), data.size());
    EXPECT_EQ(buffer.PrependableBytes(), Buffer::kCheapPrependSize);
    EXPECT_EQ(buffer.ReadableBytes(), 4096);
    EXPECT_EQ(buffer.WritableBytes(), 0);
    EXPECT_EQ(buffer.Capacity(), Buffer::kCheapPrependSize + 4096);
    EXPECT_EQ(buffer.Size(), Buffer::kCheapPrependSize + 4096);
}

TEST_F(BufferTest, Read) {
    std::string data {"HelloWorld"};
    buffer.Append(data.c_str(), data.size());

    for (char ch : data) {
        EXPECT_EQ(buffer.ReadInt8(), ch);
    }
    EXPECT_EQ(buffer.PrependableBytes(), Buffer::kCheapPrependSize);
    EXPECT_EQ(buffer.ReadableBytes(), 0);
    EXPECT_EQ(buffer.WritableBytes(), Buffer::kInitialSize);
}

TEST_F(BufferTest, Prepend) {
    std::string data {"haha"};
    buffer.Prepend(data.c_str(), data.size());
    EXPECT_EQ(buffer.PrependableBytes(), Buffer::kCheapPrependSize - data.size());
    EXPECT_EQ(buffer.ReadableBytes(), data.size());
    EXPECT_EQ(buffer.WritableBytes(), Buffer::kInitialSize);
}