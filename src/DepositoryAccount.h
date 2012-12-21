/*
 * DepositoryAccount.h
 *
 *  Created on: Nov 1, 2011
 *      Author: amcp
 */

#ifndef DEPOSITORYACCOUNT_H_
#define DEPOSITORYACCOUNT_H_

#include <vector>
#include <ql/currency.hpp>
#include <ql/time/date.hpp>
#include <ql/time/schedule.hpp>

#include "Account.h"

using namespace std;
using namespace QuantLib;

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
  DepositoryAccount(const Currency& currency, unsigned int dayOfMonth,
                    DepositType typeIn, Calendar exchCal, string nameIn)
      : Account(0 * currency, 0, exchCal, nameIn),
        type(typeIn),
        anniversary(dayOfMonth) {
  }

  ~DepositoryAccount() {
  }

  inline DepositType getType() {
    return type;
  }
  inline void setRate(Rate newRate) {
    this->rate = newRate;
  }

  Schedule generateCouponSchedule(const Date& start, const Date& end);

  string toString() {
    stringstream msg;
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

#endif /* DEPOSITORYACCOUNT_H_ */
