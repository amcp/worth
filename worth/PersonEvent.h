/*
 * PersonEvent.h
 *
 *  Created on: 23 √ÎÙ 2012
 *   Copyright 2012 Alexander Patrikalakis
 */

#ifndef WORTH_PERSONEVENT_H_
#define WORTH_PERSONEVENT_H_

#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include "MyEvent.h"
#include "Job.h"

class PersonEvent : public MyEvent {
 private:
  Person& person;
  std::string command;
  std::vector<std::string> tokens;
 public:
  PersonEvent(Person& j, std::string str, Date exec)
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

    cout << exec << ": " << command << endl;

    if (tokens[0].compare("FILE_TAX_RETURN") == 0 && tokens.size() == 3) {
      int year = 0;
      try {
        year = boost::lexical_cast<int>(tokens[1]);
      } catch (const boost::bad_lexical_cast&) {
        cerr << "Unable to parse" << endl;
        year = 0;
      }

      double uniformDeduction = 0;
      try {
        uniformDeduction = boost::lexical_cast<double>(tokens[2]);
      } catch (const boost::bad_lexical_cast&) {
        cerr << "Unable to parse" << endl;
        uniformDeduction = 0;
      }

      hash_map<std::string, QuantLib::Money, hash<std::string> > returnsByJuris = person
          .generateTaxReturn(year, uniformDeduction * person.getCurrency());

      for (hash_map<std::string, QuantLib::Money, hash<std::string> >::iterator it = returnsByJuris
          .begin(); it != returnsByJuris.end(); it++) {
        cout << "Date: " << exec << "; Depositing " << (*it).first
             << " return of " << (*it).second << endl;
        person.getMainDepository()->creditAccount((*it).second);
        cout << person.getMainDepository()->toString() << endl;
      }
    } else {
      cerr << "Unknown job event: " << command << endl;
    }
  }
};

#endif /* WORTH_PERSONEVENT_H_ */
