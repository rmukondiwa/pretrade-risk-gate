template <typename T>
#include <vector>

class RingBuffer{

    int size, rear, front, capacity;
    vector<T> buff;

    RingBuffer<T>(int cap) : size(0), rear(cap-1), front(0), capacity(cap), buff(cap)
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

    bool pop(const T& item)
    {
        if(size==0)
        {
            return false;
        }

        front = (front+1)&capacity;
        size--;
        return true;
    }

    bool isFull()
    {
        return size == capacity;
    }
}
