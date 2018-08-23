//
// Created by koushiro on 8/17/18.
//

#include <tento/net/Buffer.hpp>

NAMESPACE_BEGIN(tento)

const size_t Buffer::kCheapPrependSize = 8;
const size_t Buffer::kInitialSize = 1024;

const char Buffer::kCRLF[] = "\r\n";

NAMESPACE_END(tento)
