//
// Created by alex on 05.09.2020.
//

#ifndef SERVER_FILEREADERSANDSENDERS_H
#define SERVER_FILEREADERSANDSENDERS_H


#include <boost/asio.hpp>
#include <thread>
#include <fstream>
#include "../headers/Utils.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using boost::asio::ip::tcp;

void send404(tcp::socket* socket, const std::string &filePath){
    //PrintMutex("Err file open 404:" + filePath );
    std::string answer = "HTTP/1.1 404 Not Found\r\nConnection: closer\n\r\n";
    boost::system::error_code ignored_error;
    boost::asio::write(*socket, boost::asio::buffer(answer), ignored_error);
}



extern std::string ROOTPATH;
extern size_t MAX_CHUNK_SIZE;

void SendFileGET(std::string &fileName, tcp::socket* socket) {

    /*
    // info
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    std::string threadId = ss.str();
    */

    std::string FullPath = ROOTPATH +  fileName;

    //PrintMutex("FULL FileNeed:" + FullPath );

    std::ifstream is(FullPath.c_str(), std::ios::in | std::ios::binary);
    if (!is) {
        auto vectorSep = split(fileName, '/');
        if (vectorSep.size() >0 && vectorSep[vectorSep.size() - 1].compare("index.html") == 0) {
            send403(socket);
            return;
        }
        send404(socket, FullPath);
        return;
    }


    std::ifstream file;
    file.open(FullPath.c_str(), std::ios::binary);

    //get length of file
    file.seekg(0, std::ios::end);
    const size_t FileSize = file.tellg();
    file.seekg(0, std::ios::beg);


    // headers
    std::string dateAndTime = getDateAndTime();
    std::string answer  = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(FileSize) +
                          "\r\nContent-Type: " + getMimeType(fileName) + "\r\nDate: "+dateAndTime+"\r\nServer: AlexServer\r\nConnection: closer\r\n\r\n";

    boost::system::error_code ignoredErr;
    boost::asio::write(*socket, boost::asio::buffer(answer), ignoredErr);


    size_t chunkCount = FileSize / MAX_CHUNK_SIZE + 1;

    auto buffer = new char[MAX_CHUNK_SIZE];

    size_t getBytes = 0;
    for (int i = 0; i < chunkCount; i++) {

        size_t needRead = FileSize - getBytes ;

        if (FileSize - getBytes >= MAX_CHUNK_SIZE) {
            needRead = MAX_CHUNK_SIZE;
            file.read(buffer, needRead);
        } else {
            file.read(buffer, needRead);
        }
        getBytes += needRead;

        boost::asio::write(*socket, boost::asio::buffer(buffer, needRead), ignoredErr);
        //PrintMutex("SEND PART SIZE:" + std::to_string(needRead) +"\tTotal:"+ std::to_string(getBytes));

    }
    file.close();
    free(buffer);

    // info
    //PrintMutex("================Thread  " + threadId + " ======== end send file, Answer headers:\n" + answer );
}


void SendFileHEAD(std::string &fileName, tcp::socket* socket) {

    std::string FullPath = ROOTPATH +  fileName;

    int64_t fileSize = 0;
    struct stat fileStatbuff;
    int fd = open(FullPath.c_str(), O_RDONLY);
    if(fd == -1){
        fileSize = -1;
    }
    else{
        if ((fstat(fd, &fileStatbuff) != 0) || (!S_ISREG(fileStatbuff.st_mode))) {
            fileSize = -1;
        }
        else{
            fileSize = fileStatbuff.st_size;
        }
        close(fd);
    }


    if (fileSize == -1) {
        auto vectorSep = split(fileName, '/');
        if (vectorSep.size() > 0 && vectorSep[vectorSep.size() - 1].compare("index.html") == 0) {
            send403(socket);
            return;
        }
        send404(socket, FullPath);
        return;
    }


    // headers
    std::string dateAndTime = getDateAndTime();
    std::string answer  = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(fileSize) +
                          "\r\nContent-Type: " + getMimeType(fileName) + "\r\nDate: "+dateAndTime+"\r\nServer: AlexServer\r\nConnection: closer\r\n\r\n";

    /*
    // info
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    std::string threadId = ss.str();
    PrintMutex("================Thread  " + threadId + " ========Answer:\n" + answer );
    */

    boost::system::error_code ignored_error;
    // writing the message for current time
    boost::asio::write(*socket, boost::asio::buffer(answer), ignored_error);
}



#endif //SERVER_FILEREADERSANDSENDERS_H
