/*
 * Mortgage.h
 *
 *  Created on: 23 √ÎÙ 2012
 *      Author: amcp
 */

#ifndef MORTGAGE_H_
#define MORTGAGE_H_

#include "RecurringBill.h"
#include "Account.h"

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

#endif /* MORTGAGE_H_ */
