//
// Created by tian on 1/11/2017.
//
#include "FileUtil.h"

FileUtil::FileUtil() {
  Json::Value root;
  Json::Reader reader;
  ifstream jsonFile;
  jsonFile.open("config.json", ios::binary);

  if (!jsonFile.is_open()) {
    cout << "Cannot open the json file." << endl;
  }

  bool parsingSuccessful = reader.parse(jsonFile, root);
  //   cout << parsingSucessful << endl;
  if (parsingSuccessful) {
    // initialize rpi_id and sensor_id
    host = root["broker_address"].asString();
    string h = root["broker_port"].asString();
    port = atoi(h.c_str());
    rpi_id = root["rPi_id"].asString();
  } else {
    cout << "cannot parse configuration." << endl;
  };
  jsonFile.close();
}

int FileUtil::getPort() { return port; }

string FileUtil::getHost() { return host; }

// get rpi_id
string FileUtil::getRpiID() { return rpi_id; }
