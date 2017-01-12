//
// Created by tian on 7/13/16.
//

#include <mosquittopp.h>
#include <string.h>

#ifndef MULTITHREAD_COMMUNICATOR_H
#define MULTITHREAD_COMMUNICATOR_H

class Communicator: public mosqpp::mosquittopp
{
private:
    const char *host;
    const char *rpi_id;
    const char * topic;
    int port;
    int keep_alive;


public:
    Communicator(const char *rpi_id, const char *host, int port);

    ~Communicator();

    void on_connect(int rc);

    void on_disconnect(int rc);

    void on_publish(int mid);

    bool send_message(const char * _message);
};
#endif //MULTITHREAD_COMMUNICATOR_H
