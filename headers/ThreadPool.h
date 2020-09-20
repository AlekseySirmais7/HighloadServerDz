#ifndef SERVER_THREADPOOL_H
#define SERVER_THREADPOOL_H
#include <boost/asio.hpp>
#include <stddef.h>
#include "vector"
#include "./Utils.h"
#include "./SQueue.h"
#include "./MainHandler.h"
#include "thread"

using boost::asio::ip::tcp;

extern SQueue<tcp::socket*>* queuePtr;

class ThreadPool {

public:
    ThreadPool(size_t threadCount) {

        for (int i=0;i < threadCount; i++) {
            pool.push_back(  new std::thread(workerWork) );
        }
    }

private:

    std::vector<std::thread*> pool;

    void static workerWork() {
        tcp::socket * socketPtr;
        PrintMutex("START THREAD!");

        while (true) {
            socketPtr = queuePtr->popIfNotEmpty();
            if (socketPtr != nullptr) {
                //PrintMutex("have some work");
                MainHandler(socketPtr);
            } else {
                //PrintMutex("No work!");
            }
            usleep(300);
        }
    }

};


#endif //SERVER_THREADPOOL_H
