#include <vector>
#include <atomic>
#include <cstddef>
template <typename T>

class RingBuffer{

public:
    int size, rear, front, capacity;
    std::vector<T> buff;

    RingBuffer(int cap) : size(0), rear(cap-1), front(0), capacity(cap), buff(cap)
    {
    }

    bool push(const T& item)
    {
        if(isFull())
        {
            return false;
        }
        rear = (rear +1)&(capacity-1);
        buff[rear] = item;
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
        front = (front+1)&(capacity-1);
        size--;
        return true;
    }

    bool isFull()
    {
        return size == capacity;
    }
};
