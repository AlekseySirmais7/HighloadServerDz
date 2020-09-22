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


int main() {

    auto okConfig = readConfigAndSetGlobalVars();
    if (!okConfig) {
        exit(-1);
    }

   // threadsCount = 10;


    queuePtr = new SQueue<tcp::socket*>(nullptr);

    ThreadPool tp(threadsCount);

    boost::asio::io_service ioService;
    MyServer server(ioService );
    server.start(serverPort);
    PrintMutex("run!");
    ioService.run();
    PrintMutex("end run!");
    return 0;
}

