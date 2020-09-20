//
// Created by alex on 03.09.2020.
//

#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include "iostream"
#include <boost/asio.hpp>
#include "mutex"
#include <boost/algorithm/string/replace.hpp>

std::string createString(boost::asio::streambuf& streambuf) {
    return { boost::asio::buffers_begin(streambuf.data()), boost::asio::buffers_end(streambuf.data()) };
}


std::mutex stdOutMutex;
void PrintMutex(std::string str) {
    const std::lock_guard<std::mutex> lock(stdOutMutex);
    std::cout << str << "\n";
}


std::string urlDecode(std::string &fileNameWithPercent) {
    std::string ret;
    char ch;
    int i, ii;

    try {
        for (i = 0; i < fileNameWithPercent.length(); i++) {
            if (int(fileNameWithPercent[i]) == 37) {
                sscanf(fileNameWithPercent.substr(i + 1, 2).c_str(), "%x", &ii);
                ch = static_cast<char>(ii);
                ret += ch;
                i = i + 2;
            } else {
                ret += fileNameWithPercent[i];
            }
        }

    }

    catch (std::exception& ex) {
        PrintMutex("ERR UTILS! 1");
        std::cerr << ex.what() << std::endl;
    }

    return (ret);
}


std::string getFileName(const std::string &readData, const  int &endOfMethodIndex) {

    if (endOfMethodIndex == -1) {
        //PrintMutex("BEFORE: ERR GET FILENAME: read data:" + readData + "\n\n end Of method index:" + std::to_string(endOfMethodIndex));
        return "";
    }

    std::string fileName;
    try {

        int indexOfStartHttpVersion = readData.find("HTTP");

        if (endOfMethodIndex + 2 >= indexOfStartHttpVersion - 6 || indexOfStartHttpVersion - 6 >= readData.size() -1
        || endOfMethodIndex + 2 >= readData.size() -1) {
            //PrintMutex("ERR GET FILENAME: read data:" + readData + "\n\n end Of method index:" + std::to_string(endOfMethodIndex));
            return "";
        }

        fileName = readData.substr(endOfMethodIndex + 2, indexOfStartHttpVersion - 6);
        fileName = urlDecode(fileName);
        auto ParamStartsIndex = fileName.find("?");
        if (ParamStartsIndex != std::string::npos) {
            fileName = fileName.substr(0, ParamStartsIndex);
        }
    }

    catch (std::exception& ex) {
        PrintMutex("ERR UTILS! getFileName");
        std::cerr << ex.what() << std::endl;
    };

    return fileName;
}


//.html, .css, js, jpg, .jpeg, .png, .gif, .swf
std::map<std::string, std::string> MIMETypeMap = { {".txt", "text/plain"},
                                                   {".html", "text/html"},
                                                   {".css", "text/css"},
                                                   {".js", " text/javascript "},
                                                   {".jpg", "image/jpeg"},
                                                   {".jpeg", "image/jpeg"},
                                                   {".png", "image/png"},
                                                   {".swf", "application/x-shockwave-flash"},
                                                   {".gif", "image/gif"},
                                                   {".mp4", "video/mp4"}};

std::string getMimeType(const std::string &fileName) {

    int typeIndex = fileName.find_last_of(".");
    std::string type = fileName.substr( typeIndex, fileName.size() - 1);

    auto findResult = MIMETypeMap.find(type);
    if (findResult != MIMETypeMap.end() ) {
        return findResult->second;
    } else {
        PrintMutex("====ERR! NOT FIND:" + type);
        return "";
    }
}



using boost::asio::ip::tcp;

void send405(tcp::socket* socket) {
    //PrintMutex("SEND 405!");
    std::string answer = "HTTP/1.1 405 Not Allowed\r\nServer: AlexServer\r\nConnection: closer\r\n\r\n";
    boost::system::error_code ignoredErr;
    boost::asio::write(*socket, boost::asio::buffer(answer), ignoredErr);
}



#include <vector>
std::vector<std::string> split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::string getDateAndTime() {
    time_t now = time(0);
    std::string dateAndTimeNOtForm = ctime(& now);
    auto vectorVals = split(dateAndTimeNOtForm, ' ');
    std::string dateAndTime = vectorVals[0] + ", " + vectorVals[3] + " " + vectorVals[1] + " " + vectorVals[5] + " " + vectorVals[4] + " GMT";
    return dateAndTime;
}


void send403(tcp::socket*  socket) {
    //PrintMutex("Err Forbidden 403!" );
    std::string answer = "HTTP/1.1 403 Forbidden\r\nServer: AlexServer\r\nConnection: closer\n\r\n";
    boost::system::error_code ignoredErr;
    boost::asio::write(*socket, boost::asio::buffer(answer), ignoredErr);
}






extern size_t threadsCount;
extern std::string ROOTPATH;
#include <fstream>
bool readConfigAndSetGlobalVars() {
    std::ifstream in("/etc/httpd.conf");
    if(!in) {
        PrintMutex("ERR OPEN CONFIG FILE!");
        return false;
    }
    std::string str;
    while (std::getline(in, str)) {
        auto vectorStr =  split(str, ' ');

        if (vectorStr.size() < 2) continue;

          if (vectorStr[0].compare("cpu_limit") == 0) {
              threadsCount = atoi( vectorStr[1].c_str() );
              PrintMutex("SET cpu_limit = " + std::to_string(threadsCount));
          }
        if (vectorStr[0].compare("document_root") == 0) {
            ROOTPATH = "/app" + vectorStr[1] + "/";
            PrintMutex("SET document_root = " + ROOTPATH);
        }

    }
    in.close();
    return true;
}



#endif //SERVER_UTILS_H
