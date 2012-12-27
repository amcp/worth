/*
 * PersonEvent.h
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

#ifndef WORTH_PERSONEVENT_H_
#define WORTH_PERSONEVENT_H_

#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include "worth/MyEvent.h"
#include "worth/Job.h"

class PersonEvent : public MyEvent {
 private:
  Person& person;
  std::string command;
  std::vector<std::string> tokens;
 public:
  PersonEvent(Person& j, std::string str, QuantLib::Date exec)
      : MyEvent(exec),
        person(j),
        command(str) {
    std::stringstream strstr(str);
    std::istream_iterator<std::string> it(strstr);
    std::istream_iterator<std::string> end;
    tokens.insert(tokens.begin(), it, end);
  }

  ~PersonEvent() {
    tokens.clear();
  }

  std::string toString() const {
    return command;
  }

  void apply(Sequencer& sequencer) {
    if (tokens.size() < 1) {
      return;
    }

    std::cout << exec << ": " << command << std::endl;

    if (tokens[0].compare("FILE_TAX_RETURN") == 0 && tokens.size() == 3) {
      int year = 0;
      try {
        year = boost::lexical_cast<int>(tokens[1]);
      } catch (const boost::bad_lexical_cast&) {
        std::cerr << "Unable to parse" << std::endl;
        year = 0;
      }

      double uniformDeduction = 0;
      try {
        uniformDeduction = boost::lexical_cast<double>(tokens[2]);
      } catch (const boost::bad_lexical_cast&) {
        std::cerr << "Unable to parse" << std::endl;
        uniformDeduction = 0;
      }

      __gnu_cxx::hash_map<std::string, QuantLib::Money, __gnu_cxx::hash<std::string> > returnsByJuris = person
          .generateTaxReturn(year, uniformDeduction * person.getCurrency());

      for (__gnu_cxx::hash_map<std::string, QuantLib::Money, __gnu_cxx::hash<std::string> >::iterator it = returnsByJuris
          .begin(); it != returnsByJuris.end(); it++) {
        std::cout << "Date: " << exec << "; Depositing " << (*it).first
             << " return of " << (*it).second << std::endl;
        person.getMainDepository()->creditAccount((*it).second);
        std::cout << person.getMainDepository()->toString() << std::endl;
      }
    } else {
      std::cerr << "Unknown job event: " << command << std::endl;
    }
  }
};

#endif /* WORTH_PERSONEVENT_H_ */
