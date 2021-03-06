//
// Created by alex on 03.09.2020.
//

#ifndef SERVER_POOL_H
#define SERVER_POOL_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "mutex"
#include "iostream"
#include "../headers/Utils.h"


class threadPool {

private:

    boost::asio::io_service io_servicePool;
    boost::asio::io_service::work work;
    boost::thread_group threadsGr;
    std::size_t availableThreadCount;
    boost::mutex availableMutex;


public:

    threadPool(std::size_t poolSize ) : work(io_servicePool ), availableThreadCount(poolSize ) {
        for (std::size_t i = 0; i < poolSize; ++i ) {
            threadsGr.create_thread(boost::bind(&boost::asio::io_service::run, &io_servicePool ) );
        }
    }

    ~threadPool() {
        io_servicePool.stop();

        try {
            threadsGr.join_all();
        }
        catch ( const std::exception& ) {
            PrintMutex("ERR ~threadPool");
        }
    }

    template <typename Task>
    void runTask(Task task ) {
        //PrintMutex("runTask");
        bool needLock = false;
        availableMutex.lock();


        if (0 == availableThreadCount ) {
            availableMutex.unlock();

            //PrintMutex("no have threadSS!!");
            while (true) {

                usleep(300000); // wait while other thread ends work
                //PrintMutex("try after wait");

                availableMutex.lock();
                if (availableThreadCount > 0) {
                    availableMutex.unlock();
                    break;
                }
                availableMutex.unlock();
            }
            needLock = true;
        }

        if (needLock) {
            availableMutex.lock();
        }

        --availableThreadCount;
        availableMutex.unlock();

        // post task into the queue.
        io_servicePool.post(boost::bind(&threadPool::taskWork, this, boost::function<void()>( task ) ) );
    }

private:

    void taskWork(boost::function<void()> task ) {
        try {
            task();
        }
        catch ( const std::exception& ) {
            // send 500
        }

        boost::unique_lock< boost::mutex > lock(availableMutex );
        ++availableThreadCount;
    }
};


#endif //SERVER_POOL_H
