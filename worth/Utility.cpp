/*
 * Utility.cpp
 *
 * Created on: 23 √ÎÙ 2012
 * Copyright 2012 Alexander Patrikalakis
 *
 * This file is part of the Worth project.
 *
 * Worth is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Worth is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Worth. If not, see http://www.gnu.org/licenses/.
 */

#include "worth/Utility.h"
#include <boost/tokenizer.hpp>
#include <ql/money.hpp>
#include <ql/time/date.hpp>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace Worth {

Utility* Utility::theInstance = NULL;

// RVO, no longer need to pass output variables into
// methods by reference (also not allowed by google coding standard)
// http://cpp-next.com/archive/2009/08/want-speed-pass-by-value/
// http://en.wikipedia.org/wiki/Return_value_optimization
// http://en.wikipedia.org/wiki/Copy_elision
std::vector<std::string> Utility::readLines(const std::string& fname) {
  FILE* fp = fopen(fname.c_str(), "r");
  if (!fp) {
    fprintf(stderr, "Can't open file %s for reading: %s\n", fname.c_str(), std::strerror(errno));
    return std::vector<std::string>();
  }

  // get size of file
  fseek(fp, 0, SEEK_END);
  unsigned int len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  // abort on zero length file
  if (len == 0) {
    fclose(fp);
    fprintf(stderr, "Zero length file %s\n", fname.c_str());
    return std::vector<std::string>();
  }

  // allocate buffer size of file
  char *buf = reinterpret_cast<char *>(std::malloc(len));
  if (!buf) {
    fclose(fp);
    fprintf(stderr, "Unable to allocate %u bytes of memory for file %s\n", len, fname.c_str());
    return std::vector<std::string>();
  }

  // read whole file
  if (fread(buf, 1, len, fp) != len) {
    free(buf); buf = NULL;
    fclose(fp);
    fprintf(stderr, "Unable to read %u bytes of data from file %s into memory\n", len, fname.c_str());
    return std::vector<std::string>();
  }

  fclose(fp);

  // enclose data in string
  std::string data(buf, len);

  // tokenize on lines
  // get tokens in str
  boost::char_separator<char> sep("\n");
  boost::tokenizer<boost::char_separator<char> > tok(data, sep);

  // create list of lines
  return std::vector<std::string>(tok.begin(), tok.end());
}

std::string Utility::convertMoney(const QuantLib::Money& money) const {
  std::stringstream msg;
  msg << money;
  return msg.str();
}
std::string Utility::convertDate(const QuantLib::Date& date) const {
  std::stringstream msg;
  msg << date;
  return msg.str();
}

}
