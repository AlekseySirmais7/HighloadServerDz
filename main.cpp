#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "../headers/Pool.h"
#include "../headers/MainHandler.h"

#include <future>

using boost::asio::ip::tcp;

extern  std::string ROOTPATH = "/app/";

extern  std::string indexFilename = "index.html";

extern size_t MAX_CHUNK_SIZE = 1048576; // 1 Mb

const size_t serverPort = 80;

extern size_t threadsCount = 4;



int main() {

    auto okConfig = readConfigAndSetGlobalVars();
    if (!okConfig) {
        exit(-1);
    }

    threadPool ThreadPool(threadsCount);

    try {

        boost::asio::io_service io_service;

        std::cout << "start serving at " << serverPort << std::endl;

        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), serverPort));


        while (true) {

            tcp::socket* socketPtr = new tcp::socket(io_service);




            acceptor.accept(*socketPtr);
            //acceptor.async_accept(socket, handler);



            ThreadPool.queuePtr->push(socketPtr);

        }
    }

    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }

    return 0;
}

