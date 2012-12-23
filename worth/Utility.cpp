/*
 * Utility.cpp
 *
 *  Created on: 23 √ÎÙ 2012
 *      Author: amcp
 */

#include "Utility.h"
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <stdexcept>
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>

Worth::Utility* Worth::Utility::theInstance = NULL;

// RVO, no longer need to pass output variables into methods by reference (also not allowed by google coding standard)
// http://cpp-next.com/archive/2009/08/want-speed-pass-by-value/
// http://en.wikipedia.org/wiki/Return_value_optimization
// http://en.wikipedia.org/wiki/Copy_elision
std::vector<std::string> Worth::Utility::readLines(const std::string& fname) {
  FILE* fp = fopen("data/2013_CA_SINGLE.txt.dat","r");

  if (!fp) {
    throw std::runtime_error("Can't open file: " + std::string(std::strerror(errno)));
  }

  //get size of file
  fseek(fp, 0, SEEK_END);
  size_t len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  //abort on zero length file
  if (len == 0) {
    fclose(fp);
    throw std::runtime_error("Zero length file");
  }

  //allocate buffer size of file
  char *buf = (char *) std::malloc(len);
  if (!buf) {
    fclose(fp);
    throw std::runtime_error("Can't allocate memory");
  }

  //read whole file
  if (fread(buf, 1, len, fp) != len) {
    std::free(buf);
    fclose(fp);
    throw std::runtime_error("Couldn't read entire file");
  }

  fclose(fp);

  //enclose data in string
  std::string data(buf, len);

  //tokenize on lines
  //get tokens in str
  boost::char_separator<char> sep("\n");
  boost::tokenizer<boost::char_separator<char> > tok(data, sep);

  //create list of lines
  return std::vector<std::string> (tok.begin(), tok.end());
}
