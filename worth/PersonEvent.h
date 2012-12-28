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

#include <boost/tokenizer.hpp>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "worth/DepositoryAccount.h"
#include "worth/MyEvent.h"
#include "worth/Job.h"

namespace Worth {

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
    boost::char_separator<char> sep(", ");
    boost::tokenizer<boost::char_separator<char> > tok(str, sep);
    std::copy(tok.begin, tok.end, tokens.begin());
  }

  ~PersonEvent() {
    tokens.clear();
  }

  std::string toString() const {
    return command;
  }

  void apply(Sequencer* sequencer) {
    if (tokens.size() < 1) {
      return;
    }

    printf("%s: %s\n", exec, command.c_str());

    if (tokens[0].compare("FILE_TAX_RETURN") == 0 && tokens.size() == 3) {
      int year = 0;
      try {
        year = boost::lexical_cast<int>(tokens[1]);
      } catch (const boost::bad_lexical_cast&) {
        fprintf(stderr, "Unable to parse %s as an int.\n", tokens[1].c_str());
        year = 0;
      }

      double uniformDeduction = 0;
      try {
        uniformDeduction = boost::lexical_cast<double>(tokens[2]);
      } catch (const boost::bad_lexical_cast&) {
        fprintf(stderr, "Unable to parse %s as a double.\n", tokens[2].c_str());
        uniformDeduction = 0.0;
      }

      JobPayment::StringMoneyMap returnsByJuris = person
          .generateTaxReturn(year, uniformDeduction * person.getCurrency());

      JobPayment::StringMoneyMap::iterator it;
      for (it = returnsByJuris.begin(); it != returnsByJuris.end(); it++) {
        printf("Date: %s; Depositing %s return of %s\n", exec, it->first.c_str(), it->second);
        QuantLib::Money returnMoney = (*it).second;
        person.getMainDepository()->creditAccount(returnMoney);
        printf("%s\n", person.getMainDepository()->toString().c_str());
      }
    } else {
      fprintf(stderr, "Unknown job event: %s\n", command.c_str());
    }
  }
};

}

#endif /* WORTH_PERSONEVENT_H_ */
