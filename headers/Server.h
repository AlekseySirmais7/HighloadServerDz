//
// Created by alex on 03.09.2020.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "mutex"
#include "iostream"
#include "../headers/Utils.h"

#include "./SQueue.h"
#include "./MainHandler.h"



using namespace boost::asio;

extern SQueue<tcp::socket*>* queuePtr;


class MyServer {

public:
    MyServer(io_service& io_service) : ioService(io_service ), acceptor(new ip::tcp::acceptor(io_service) ) {}

    bool start(int port) {
        ip::tcp::endpoint endpoint(ip::tcp::v4(), port);
        acceptor->open(endpoint.protocol());
        acceptor->set_option(ip::tcp::acceptor::reuse_address(true));
        acceptor->bind(endpoint);
        acceptor->listen();

        ip::tcp::socket* newSocket = new ip::tcp::socket(ioService );
        acceptor->async_accept(
                *newSocket,
                boost::bind(
                        &MyServer::NewAcceptAndPushSocket,
                        this,
                        newSocket,
                        boost::asio::placeholders::error
                )
        );


    }

    void NewAcceptAndPushSocket(ip::tcp::socket* oldSocket, const boost::system::error_code& error) {

        ip::tcp::socket* newSocket = new ip::tcp::socket(ioService );
        acceptor->async_accept(
                *newSocket,
                boost::bind(
                        &MyServer::NewAcceptAndPushSocket,
                        this,
                        newSocket,
                        boost::asio::placeholders::error
                )
        );
        queuePtr->push(oldSocket);
    }

private:
    io_service& ioService;
    ip::tcp::acceptor* acceptor;
};

#endif
