#ifndef QUEUE_H
#define QUEUE_H



#include <queue>
#include <mutex>
#include <memory>

template <class T>
class SQueue {
public:
    SQueue() = delete;
    explicit SQueue(const T &element404);
    ~SQueue();
     void push(const  T element);
    T  popIfNotEmpty();

    //SQueue(const SQueue&) = delete;
    //SQueue(const SQueue&&) = delete;
    //SQueue& operator=(const SQueue&) = delete;
    //SQueue& operator=(const SQueue&&) = delete;

private:
    T element404;
    std::unique_ptr<std::queue<T>> queue;
    std::mutex GQueueMutex;
};


template <class T>
SQueue<T>::~SQueue() {}

template <class T>
SQueue<T>::SQueue(const T &element404) {
    queue.reset(new std::queue<T>);
    this->element404 =  element404;
}

template <class T>
 void SQueue<T>::push(const T element) {
    const std::lock_guard<std::mutex> lock(GQueueMutex);
    queue->push(element);
}

template <class T>
T SQueue<T>::popIfNotEmpty() {
    const std::lock_guard<std::mutex> lock(GQueueMutex);
    T answer = this->element404;
    if (!queue->empty()) {
        answer = queue->front();
        queue->pop();
    }
    return answer;
}

#endif // QUEUE_H
