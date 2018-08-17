//
// Created by koushiro on 8/17/18.
//

#ifndef TENTO_NONCOPYABLE_HPP
#define TENTO_NONCOPYABLE_HPP

namespace tento {

class NonCopyable {
public:
    NonCopyable(const NonCopyable &) = delete;
    NonCopyable &operator=(const NonCopyable &) = delete;

protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

} // namespace tento

#endif //TENTO_NONCOPYABLE_HPP
