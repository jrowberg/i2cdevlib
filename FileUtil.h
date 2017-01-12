// FileUtil//
//  FileUtil.h
//  adxl345_cpp
//
//  Created by TianLiu on 1/11/2017.
//  Copyright © 2016 TianLiu. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <json/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

#ifndef FILEUTIL_H
#define FILEUTIL_H
class FileUtil {
public:
  void load_configuration();
  int getPort();
  string getHost();
  string getRpiID();

private:
  string host;
  int port;
  string rpi_id;
};

#endif
