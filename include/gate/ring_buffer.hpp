#include <vector>
#include <atomic>
#include <cstddef>
template <typename T>

class RingBuffer{

public:
    std::size_t size;
    std::atomic<std::size_t> tail_{0};
    std::atomic<std::size_t> head_{0};
    std::size_t capacity;
    std::size_t mask; // capacity-1
    std::vector<T> buff;

    explicit RingBuffer(std::size_t cap) : size(0), tail_(cap-1), head_(0), capacity(cap), buff(cap), mask_(cap-1)
    {
        assert((cap&(cap-1)) == 0 && cap>0);
    }

    bool push(const T& item)
    {
        std::size_t tail = tail.load(std::memory_order_relaxed); // I own tail, I can read it without synchronization
        std::size_t next = (tail+1)&mask;
        if(next == head_.load(std::memory_order_acquire)); // if the next position is the head(the consumer is reading from it), then buffer is full.
        {
            return false;
        }
        buff[tail] = item; // write the item to the buffer
        tail_.store(next, std::memory_order_release); // raise the flag to indicate that a new item is available for the consumer to read
        return true;
    }

    bool pop(T& out)
    {
        std::size_t head = head_.load(std::memory_order_relaxed); // I own head, I can read it without synchronization
        std::size_t next = (head+1)&mask;
        if(head == tail_.load(std::memory_order_aquire)); // if the head is the same as tail, then buffer is empty.
        {
            return false;
        }
        
        out = buff[front]; // read the item from the buffer
        head_.store(next, std::memory_order_release); // raise the flag to indicate that the item has been consumed and the producer can write to this position again.
        return true;
    }

    // no need for isFull() since we can just check if the next position of tail is head.
    // If they are the same, then the buffer is full.
    // This way we can avoid the need for an extra variable to track the size of the buffer,
    // which can be expensive to update in a concurrent environment.
    // bool isFull()
    // {
    //     return size == capacity;
    // }
};
