#ifndef __MYINTERVAL_H__
#define __MYINTERVAL_H__

#include <thread>

using namespace std;

template <typename T, typename Fn, typename... Args>
void func_interval(T t, Fn fn, Args... args) {
    while (true) {
        fn(args...);
        this_thread::sleep_for(t);
    }
}
struct myinterval {
    thread routine;

    template <typename T, typename Fn, typename... Args>
    myinterval(T t, Fn fn, Args... args) : routine(func_interval<T, Fn, Args...>, t, fn, args...) {}
    ~myinterval() {
        pthread_cancel(routine.native_handle());
        routine.join();
    }
};

#endif
