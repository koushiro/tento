//
// Created by koushiro on 8/17/18.
//

#ifndef TENTO_SINGLETON_HPP
#define TENTO_SINGLETON_HPP

namespace tento {

template<typename T>
class Singleton {
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static T& GetInstance() {
        static T instance;
        return instance;
    }

private:
    Singleton() = default;
    ~Singleton() = default;
};

} // namespace tento

#endif //TENTO_SINGLETON_HPP
