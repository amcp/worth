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
#include <ql/money.hpp>
#include <ql/time/date.hpp>
#include <ql/time/schedule.hpp>

#include <vector>
#include <string>
#include <sstream>

#include "worth/Utility.h"

namespace Worth {

enum DepositType {
  CASH,
  RETIREMENT,
  IRA
};

class DepositoryAccount {
 private:
  QuantLib::Money balance;
  QuantLib::Rate rate;
  QuantLib::Calendar exchangeCalendar;
  std::string name;
  DepositType type;
  unsigned int anniversary;

 public:
  DepositoryAccount(const QuantLib::Currency& currency, unsigned int dayOfMonth,
                    DepositType typeIn, QuantLib::Calendar exchCal,
                    std::string nameIn)
      : balance(0 * currency),
        rate(0),
        exchangeCalendar(exchCal),
        name(nameIn),
        type(typeIn),
        anniversary(dayOfMonth) {
  }

  ~DepositoryAccount();

  inline DepositType getType() {
    return type;
  }
  inline void setRate(QuantLib::Rate newRate) {
    this->rate = newRate;
  }

  inline QuantLib::Money getBalance() {
    return balance;
  }
  inline QuantLib::Rate getRate() {
    return rate;
  }
  inline QuantLib::Calendar getCalendar() {
    return exchangeCalendar;
  }
  inline QuantLib::Currency getCurrency() {
    return balance.currency();
  }

  void debitAccount(QuantLib::Money amt);
  void creditAccount(QuantLib::Money amt);
  std::string toString();
  QuantLib::Schedule generateCouponSchedule(const QuantLib::Date& start,
                                            const QuantLib::Date& end);
};
}

#endif  // WORTH_DEPOSITORYACCOUNT_H_
