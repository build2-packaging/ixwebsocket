/*
 *  main.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2020 Machine Zone, Inc. All rights reserved.
 *
 *  Super simple standalone example. See ws folder, unittest and doc/usage.md for more.
 *
 *  On macOS
 *  $ mkdir -p build ; cd build ; cmake -DUSE_TLS=1 .. ; make -j ; make install
 *  $ clang++ --std=c++14 --stdlib=libc++ main.cpp -lixwebsocket -lz -framework Security -framework Foundation
 *  $ ./a.out
 *
 *  Or use cmake -DBUILD_DEMO=ON option for other platform
 */

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXUserAgent.h>
#include <iostream>

int main()
{
    // Required on Windows
    ix::initNetSystem();

    // Our websocket object
    ix::WebSocket webSocket;

    // Connect to a server with encryption
    // See https://machinezone.github.io/IXWebSocket/usage/#tls-support-and-configuration
    std::string url("wss://ws.ifelse.io");
    webSocket.setUrl(url);

    std::cout << ix::userAgent() << std::endl;
    std::cout << "Connecting to " << url << "..." << std::endl;

    int status = 1;

    // Setup a callback to be fired (in a background thread, watch out for race conditions !)
    // when a message or an event (open, close, error) is received
    webSocket.setOnMessageCallback([&status](const ix::WebSocketMessagePtr& msg)
        {
            if (msg->type == ix::WebSocketMessageType::Message)
            {
                if (msg->str == "hello world" && status == -2) {
                  status = -1;
                }
            }
            else if (msg->type == ix::WebSocketMessageType::Open)
            {
                if (status == 1) {
                  status = -2;
                }
            }
            else if (msg->type == ix::WebSocketMessageType::Error)
            {
                if (status != 0) {
                  status = 0;
                }
            }
        }
    );

    // Now that our callback is setup, we can start our background thread and receive messages
    webSocket.start();

    // Send a message to the server (default to TEXT mode)
    webSocket.send("hello world");

    std::this_thread::sleep_for(std::chrono::seconds{2});

    return status;
}
