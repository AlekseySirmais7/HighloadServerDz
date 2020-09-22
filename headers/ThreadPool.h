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

extern boost::asio::io_service * ioServicePtr;
extern ip::tcp::acceptor* acceptorPtr;


class ThreadPool {

public:
    ThreadPool(size_t threadCount) {

        for (int i=0;i < threadCount; i++) {
            pool.push_back( std::thread(NewWorkerWork)  );
            pool.push_back(std::thread(NewWorkerWorkHandler)  );
        }
    }

private:

    std::vector<std::thread> pool;


    void static NewWorkerWork() {


        while (true) {
            ip::tcp::socket* newSocket = new ip::tcp::socket(*ioServicePtr );

            std::function<void (ip::tcp::socket*, SQueue<ip::tcp::socket*> * ptr)> f = [](ip::tcp::socket* x, SQueue<ip::tcp::socket*> * ptr){

                ptr->push(x);

            };

            acceptorPtr->async_accept(
                    *newSocket,
                    boost::bind(
                            f,
                            newSocket,
                            queuePtr
                    )
            );


            //acceptorPtr->accept(*newSocket);

            queuePtr->push(newSocket);

            //MainHandler(newSocket);

        }

    }

    void static NewWorkerWorkHandler() {

        tcp::socket * socketPtr;
        PrintMutex("START THREAD handler");

        while (true) {
            socketPtr = queuePtr->popIfNotEmpty();
            if (socketPtr != nullptr) {
                MainHandler(socketPtr);
            } else {
            }
            //usleep(300);

        }



    }




};


#endif //SERVER_THREADPOOL_H
