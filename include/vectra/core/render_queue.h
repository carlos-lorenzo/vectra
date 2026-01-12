#ifndef VECTRA_RENDER_QUEUE_H
#define VECTRA_RENDER_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <utility>
#include <cstddef>

// A simple thread-safe queue template
template <typename T>
class RenderQueue
{

public:
    explicit RenderQueue(std::size_t capacity = 3) : capacity_(capacity), active_(true) {}

    void shutdown()
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            active_ = false;
        }
        not_full_.notify_all();
        not_empty_.notify_all();
    }

    void wait_for_push(T value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        not_full_.wait(lock, [this]() { return queue_.size() < capacity_ || !active_; });
        if (!active_) return;
        queue_.push(std::move(value));
        not_empty_.notify_one();
    }
    // Align with Engine usage; blocks until an item is enqueued
    void push(T value)
    {
        wait_for_push(std::move(value));
    }
    void wait_for_pop(T &value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_.wait(lock, [this]() { return !queue_.empty() || !active_; });
        if (!active_ && queue_.empty()) return;
        value = std::move(queue_.front());
        queue_.pop();
        not_full_.notify_one();
    }
    // Align with Engine usage; blocks until an item is available
    void pop(T &value)
    {
        wait_for_pop(value);
    }

private:
    std::size_t capacity_;
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    bool active_;

};

#endif //VECTRA_RENDER_QUEUE_H