#include <vector>
#include <atomic>
#include <cstddef>
template <typename T>

class RingBuffer{

public:
    std::size_t size;
    std::atomic<std::size_t> tail{0};
    std::atomic<std::size_t> head{0};
    std::size_t capacity;
    std::size_t mask; // capacity-1
    std::vector<T> buff;

    explicit RingBuffer(std::size_t cap) : size(0), tail(cap-1), head(0), capacity(cap), buff(cap), mask(cap-1)
    {
        assert((cap&(cap-1)) == 0 && cap>0);
    }

    bool push(const T& item)
    {
        if(isFull())
        {
            return false;
        }
        tail = (tail +1)&(capacity-1);
        buff[tail] = item;
        size++;
        return true;
    }

    bool pop(const T& out)
    {
        if(size==0)
        {
            return false;
        }
        
        out = buff[front];
        head = (head+1)&(capacity-1);
        size--;
        return true;
    }

    bool isFull()
    {
        return size == capacity;
    }
};
