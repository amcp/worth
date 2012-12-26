/*
 * Mortgage.h
 *
 *  Created on: 23 √ÎÙ 2012
 *  Copyright 2012 Alexander Patrikalakis
 */

#ifndef WORTH_MORTGAGE_H_
#define WORTH_MORTGAGE_H_

#include <string>

#include "worth/RecurringBill.h"
#include "worth/Account.h"



class Mortgage : public RecurringBill {
 protected:
  Money closingCosts;
  Account balance;

 public:
  Mortgage(string nameIn, Money amountIn, Date startDate, Date endDate,
           Calendar calendar, DepositoryAccount* payer, Money closing,
           Rate rate, Money startingBalance, Person& user)
      : RecurringBill(nameIn, amountIn, Period(1, Months), startDate, endDate,
                      calendar, payer, user),
        balance(Account(startingBalance, rate, calendar, nameIn)) {
  }

  ~Mortgage() {
  }

  inline PartiallyDeductiblePayment* getNextBillPayment() {
    Money interest = -1 * balance.getBalance() * balance.getRate() / 12.0;
    Money principal = amount - interest;
    return new PartiallyDeductiblePayment(*paymentIterator, amount, interest);
  }

  Account& getUnderlyingLiability() {
    return balance;
  }
};

#endif  // WORTH_MORTGAGE_H_
