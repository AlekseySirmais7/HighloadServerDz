//
// Created by alex on 03.09.2020.
//

#ifndef SERVER_MAINHANDLER_H
#define SERVER_MAINHANDLER_H

#include <boost/asio.hpp>
//#include <thread>
#include <fstream>
#include "../headers/Utils.h"
#include "../headers/FileReadersAndSenders.h"

using boost::asio::ip::tcp;

extern std::string indexFilename;

void MainHandler(tcp::socket* socket) {

    /*
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    std::string threadId = ss.str();
    PrintMutex("                                                      Thread  " + threadId + ": get task");
    */


    boost::asio::streambuf buffer;
    boost::system::error_code errCode;

    boost::asio::read(*socket, buffer,boost::asio::transfer_at_least(1), errCode);

    std::string readFromClientStr = createString(buffer);


    //PrintMutex("READ:\n" + readFromClientStr);


    int endOfMethodIndex = readFromClientStr.find_first_of(' ');
    std::string method = readFromClientStr.substr(0, endOfMethodIndex);


    //PrintMutex("Method:" + method );


    std::string fileName = getFileName(readFromClientStr, endOfMethodIndex);

    if (fileName.compare("SomeTrashCase")==0) {
        goto FREESOCKET;
    }

    //PrintMutex("FileName:" + fileName );


    if (fileName.size() == 0) {
        fileName += indexFilename;
    } else

    if (fileName[fileName.size() -1] == '/') {
        fileName += indexFilename;
    }


    if (fileName.find("../") != std::string::npos) {
        //PrintMutex("Err Forbidden 403!" );
        std::string answer = "HTTP/1.1 403 Forbidden\r\nConnection: closer\n\r\n";
        boost::system::error_code ignoredErr;
        boost::asio::write(*socket, boost::asio::buffer(answer), ignoredErr);

    } else if ( method.compare("GET") == 0) {

        //PrintMutex("METHOD GET!" );

        SendFileGET(fileName, socket);

    } else if ( method.compare("HEAD") == 0) {

        //PrintMutex("METHOD HEAD!" );

        fileName = fileName.substr(0, fileName.size()-1);

        SendFileHEAD(fileName, socket);

    } else {
        send405(socket);
    }



    FREESOCKET:
    socket->shutdown(boost::asio::socket_base::shutdown_both);
    socket->close();
    free(socket);


    // info
    //PrintMutex("Thread  " + threadId + ":  END WORK" );
    //usleep(5000000);
    //PrintMutex("Thread  " + threadId + ":  Wake up!" );
};



#endif //SERVER_MAINHANDLER_H
