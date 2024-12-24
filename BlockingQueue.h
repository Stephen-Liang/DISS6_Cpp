#pragma once
#ifndef BLOCKING_QUEUE_H
#define BLOCKING_QUEUE_H
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <limits>  // ȷ���������ͷ�ļ�
#include <cstddef> // ���� size_t ����
template <typename T>
class BlockingQueue {
private:
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable condition;
    size_t max_size;
public:
    // Constructor with optional maximum size
    BlockingQueue() : max_size(~size_t(0)) {} // ʹ��λ�����ȡ���ֵ
    explicit BlockingQueue(size_t max_size) : max_size(max_size) {}
    // Dequeue operation with blocking
    T Dequeue() {
        std::unique_lock<std::mutex> lock(mutex);
        // Wait until queue is not empty
        condition.wait(lock, [this]() { return !queue.empty(); });
        // Get and remove the front element
        T item = std::move(queue.front());
        queue.pop();
        // Notify any waiting threads that space is available
        condition.notify_one();
        return item;
    }
    // Enqueue operation with optional blocking
    template <typename U = T>
    void Enqueue(T data) {
        // ʹ�� SFINAE ��������ͬ����
        if constexpr (std::is_pointer_v<U>) {
            if (data == nullptr) {
                throw std::invalid_argument("Cannot enqueue null pointer");
            }
        }
        std::unique_lock<std::mutex> lock(mutex);
        // �ȴ�����δ��
        condition.wait(lock, [this]() { return queue.size() < max_size; });
        // ʹ������ת��
        queue.push(std::forward<T>(data));
        // ֪ͨ�ȴ����߳�
        condition.notify_one();
    }
    // Get current queue size
    size_t Length() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }
    // Check if queue is empty
    bool IsEmpty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }
};

#endif // BLOCKING_QUEUE_H