#ifndef THREAD_STARTER_H
#define THREAD_STARTER_H

template<class T, void(T::*thread)()>
void* start_thread(void* ptr)
{
    (static_cast<T*>(ptr)->*thread)();
    return 0;
}

#endif // THREAD_STARTER_H
