#include <iostream>
#include <boost/asio.hpp>
#include "../headers/Server.h"
#include "../headers/ThreadPool.h"

SQueue<tcp::socket*> * queuePtr;

using boost::asio::ip::tcp;

extern  std::string ROOTPATH = "/app/";

extern  std::string indexFilename = "index.html";

extern size_t MAX_CHUNK_SIZE = 1048576; // 1 Mb

const size_t serverPort = 8080;

extern size_t threadsCount = 4;

boost::asio::io_service * ioServicePtr;
ip::tcp::acceptor* acceptorPtr;

int main() {

    auto okConfig = readConfigAndSetGlobalVars();
    if (!okConfig) {
        exit(-1);
    }

   // threadsCount = 10;


    queuePtr = new SQueue<tcp::socket*>(nullptr);


    boost::asio::io_service ioService;

    ioServicePtr = &ioService;



    MyServer server(ioService );

    acceptorPtr = server.acceptor;

    server.start(serverPort);



    ThreadPool tp(threadsCount);


    PrintMutex("run!");
    ioService.run();

    while (true) {
        usleep(300000000);
    }

    PrintMutex("end run!");
    return 0;
}

