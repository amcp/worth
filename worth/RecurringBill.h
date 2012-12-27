/*
 * RecurringBill.h
 *
 *  Created on: 23 √ÎÙ 2012
 *   Copyright 2012 Alexander Patrikalakis
 */

#ifndef WORTH_RECURRINGBILL_H_
#define WORTH_RECURRINGBILL_H_

#include <ql/time/date.hpp>
#include <ql/time/schedule.hpp>
#include <ql/money.hpp>
#include <string>

#include "worth/Utility.h"
#include "worth/Payment.h"
#include "worth/Job.h"

class RecurringBill {
 protected:
  std::string name;
  QuantLib::Money amount;
  QuantLib::Schedule schedule;
  QuantLib::Schedule::const_iterator paymentIterator;
  bool active;
  DepositoryAccount* payingAccount;
  Person& user;

 public:
  RecurringBill(std::string nameIn, QuantLib::Money amountIn, QuantLib::Period tenor,
                QuantLib::Date startDate, QuantLib::Date endDate,
                QuantLib::Calendar calendar, DepositoryAccount* payer,
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

  inline const QuantLib::Schedule& getSchedule() const {
    return schedule;
  }

  inline const std::string& getName() const {
    return name;
  }

  inline bool isActive() const {
    return active;
  }

  inline QuantLib::Money getAmount() const {
    return amount;
  }
  inline void setAmount(QuantLib::Money m) {
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
  inline QuantLib::Date getCurrentPaymentDate() const {
    return *paymentIterator;
  }

  DepositoryAccount* getPayingAccount() {
    return payingAccount;
  }

  Person& getUser() {
    return user;
  }
};

#endif  // WORTH_RECURRINGBILL_H_
