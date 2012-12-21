/*
 * RecurringBill.h
 *
 *  Created on: 23 √ÎÙ 2012
 *      Author: amcp
 */

#ifndef RECURRINGBILL_H_
#define RECURRINGBILL_H_

#include <string>
#include <ql/time/date.hpp>
#include <ql/time/schedule.hpp>
#include <ql/money.hpp>

#include "Utility.h"
#include "Payment.h"
#include "Job.h"

using namespace std;
using namespace QuantLib;

class RecurringBill {
 protected:
  string name;
  Money amount;
  Schedule schedule;
  Schedule::const_iterator paymentIterator;
  bool active;
  DepositoryAccount* payingAccount;
  Person& user;

 public:
  RecurringBill(string nameIn, Money amountIn, Period tenor, Date startDate,
                Date endDate, Calendar calendar, DepositoryAccount* payer,
                Person& usr)
      : name(nameIn),
        amount(amountIn),
        active(true),
        user(usr) {
    QL_REQUIRE(payer != NULL, "Paying account must not be NULL.");
    payingAccount = payer;
    schedule = Worth::Utility::getInstance()->generateSchedule(tenor, startDate,
                                                               endDate,
                                                               calendar);
    paymentIterator = schedule.begin();
  }

  inline const Schedule& getSchedule() const {
    return schedule;
  }

  inline const string& getName() const {
    return name;
  }

  inline bool isActive() const {
    return active;
  }

  inline Money getAmount() const {
    return amount;
  }
  inline void setAmount(Money m) {
    amount = m;
  }

  inline Payment* getNextBillPayment() {
    Payment* result = new Payment(*paymentIterator, amount);
    paymentIterator++;
    return result;
  }

  inline bool hasMoreBillPayments() const {
    return paymentIterator != schedule.end();
  }
  inline Date getCurrentPaymentDate() const {
    return *paymentIterator;
  }

  DepositoryAccount* getPayingAccount() {
    return payingAccount;
  }

  Person& getUser() {
    return user;
  }
};

#endif /* RECURRINGBILL_H_ */
