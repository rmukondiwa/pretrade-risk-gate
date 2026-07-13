#include <vector>
#include <atomic>
#include <cstddef>
template <typename T>

class RingBuffer{

public:
    int size, rear, front, capacity;
    std::vector<T> buff;

    RingBuffer(int cap) : size(0), tail(cap-1), head(0), capacity(cap), buff(cap)
    {
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
