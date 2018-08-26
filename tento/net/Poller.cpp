//
// Created by koushiro on 8/26/18.
//

#include <tento/net/Poller.hpp>

#include <tento/net/PollPoller.hpp>
#include <tento/net/EPollPoller.hpp>

NAMESPACE_BEGIN(tento)
NAMESPACE_BEGIN(net)

Poller* Poller::NewDefaultPoller(EventLoop* loop) {
#if 0
    return new EPollPoller(loop);
#else
    return new PollPoller(loop);
#endif
}

NAMESPACE_END(net)
NAMESPACE_END(tento)