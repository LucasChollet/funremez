//
//  LolRemez — Remez algorithm implementation
//
//  Copyright © 2005–2022 Sam Hocevar <sam@hocevar.net>
//
//  This program is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <queue>
#include <utility>

template<typename T>
class ThreadQueue
{
public:
    ThreadQueue() = default;
    ThreadQueue(ThreadQueue const &) = delete;
    ThreadQueue &operator=(ThreadQueue const &) = delete;

    std::size_t size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    /* Push a value to the back of the queue. This is an unbounded queue,
     * so unlike the bounded queue it replaces, push() never blocks. */
    void push(T value)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(std::move(value));
        }
        m_cond.notify_one();
    }

    /* Pop a value from the front of the queue, blocking until one is
     * available. */
    T pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cond.wait(lock, [this] { return !m_queue.empty(); });
        T value = std::move(m_queue.front());
        m_queue.pop();
        return value;
    }

    /* Pop a value from the front of the queue without blocking. Returns
     * false if the queue is empty. */
    bool try_pop(T &ret)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty())
            return false;
        ret = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

private:
    std::queue<T> m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_cond;
};
