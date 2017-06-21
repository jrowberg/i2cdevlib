//
// Created by tian on 7/13/16.
//

#include "Communicator.h"
#include <iostream>
using namespace std;

// constructor that does initialization and start a new thread to process
// network traffic.
Communicator::Communicator(const char *rpi_id, const char *host, int port)
    : mosquittopp(rpi_id) {
  int keep_alive = 60;
  mosqpp::lib_init();
  connect(host, port, keep_alive);
  //    cout << connect(host, port, keep_alive);
  loop_start();
};

// destructor that stops connecting to the broker
Communicator::~Communicator() {
  //   block until the network thread finishes
  loop_stop();
  mosqpp::lib_cleanup();
}

void Communicator::on_connect(int rc) {
  if (rc == 0) {
    cout << "Connected with server" << endl;
  } else {
    cout << "Impossible to connect with server" << endl;
  }
}

void Communicator::on_disconnect(int rc) { cout << "disconnection" << endl; }

void Communicator::on_publish(int mid) {
//   cout << "Message succeed to be published" << endl;
}

bool Communicator::send_message(const char *_message) {
  int ret = publish(NULL, "vibe", strlen(_message), _message, 1, false);
  return (ret == MOSQ_ERR_SUCCESS);


}
