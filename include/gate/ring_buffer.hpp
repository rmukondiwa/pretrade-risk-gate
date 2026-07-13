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
        if(isFull())
        {
            return false;
        }
        tail = (tail +1)&mask;
        buff[tail] = item;
        size++;
        return true;
    }

    bool pop(T& out)
    {
        if(size==0)
        {
            return false;
        }
        
        out = buff[front];
        head = (head+1)&mask;
        size--;
        return true;
    }

    bool isFull()
    {
        return size == capacity;
    }
};
