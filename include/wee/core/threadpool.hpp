#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace atom {
class threadpool 
{
    std::vector<std::thread> _workers;
    std::queue<std::function<void()> > _tasks;
    std::mutex _mtx;
    std::condition_variable _cv;
    bool _stop;
public:
    threadpool() : _stop(false) 
    {
        size_t threads = std::thread::hardware_concurrency();
        for(size_t i = 0;i < threads;++i)
            _workers.emplace_back(
            [this]
            {
                for(;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(_mtx);
                        _cv.wait(lock,
                            [this]{ return _stop || !_tasks.empty(); });
                        if(_stop && _tasks.empty())
                            return;
                        task = _tasks.front();
                        _tasks.pop();
                    }
                    task();
                }
            }
        );
    }

    virtual ~threadpool() {
        {
            std::unique_lock<std::mutex> lock(_mtx);
            _stop = true;
        }
        _cv.notify_all();
        for(std::thread &worker: _workers)
        worker.join();
    }

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) { 
        using return_type = decltype(f(args...));

        auto task = std::make_shared< std::packaged_task<return_type(Args...)> >(
           std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
            
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(_mtx);

            // don't allow enqueueing after stopping the pool
            if(_stop)
                throw std::runtime_error("enqueue on stopped threadpool");

            _tasks.emplace([task, &args...](){ (*task)(args...); });
        }
        _cv.notify_one();
        return res;
    }
};

/*
int main(int , char**) {
    threadpool tp;
    auto res = tp.enqueue([] (int a) {
        return a * 2;
    }, 10);
    
    return res.get();
}*/
}
