/*
 * DepositoryAccount.h
 *
 * Created on: Nov 1, 2011
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

#ifndef WORTH_DEPOSITORYACCOUNT_H_
#define WORTH_DEPOSITORYACCOUNT_H_

#include <ql/currency.hpp>
#include <ql/time/date.hpp>
#include <ql/time/schedule.hpp>

#include <vector>
#include <string>
#include <sstream>

#include "worth/Account.h"

enum DepositType {
  CASH,
  RETIREMENT,
  IRA
};

class DepositoryAccount : public Account {
 private:
  DepositType type;
  unsigned int anniversary;

 public:
  DepositoryAccount(const QuantLib::Currency& currency, unsigned int dayOfMonth,
                    DepositType typeIn, QuantLib::Calendar exchCal,
                    std::string nameIn)
      : Account(0 * currency, 0, exchCal, nameIn),
        type(typeIn),
        anniversary(dayOfMonth) {
  }

  ~DepositoryAccount() {
  }

  inline DepositType getType() {
    return type;
  }
  inline void setRate(QuantLib::Rate newRate) {
    this->rate = newRate;
  }

  QuantLib::Schedule generateCouponSchedule(const QuantLib::Date& start,
                                            const QuantLib::Date& end);

  std::string toString() {
    std::stringstream msg;
    msg << "Type: ";
    if (type == CASH) {
      msg << "CASH";
    } else if (type == RETIREMENT) {
      msg << "RETIREMENT";
    } else if (type == IRA) {
      msg << "IRA";
    }
    msg << "; " << Account::toString();
    return msg.str();
  }
};

#endif  // WORTH_DEPOSITORYACCOUNT_H_
