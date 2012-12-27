/*
 * Mortgage.h
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

#ifndef WORTH_MORTGAGE_H_
#define WORTH_MORTGAGE_H_

#include <string>

#include "worth/RecurringBill.h"
#include "worth/Account.h"



class Mortgage : public RecurringBill {
 protected:
  QuantLib::Money closingCosts;
  Account balance;

 public:
  Mortgage(std::string nameIn, QuantLib::Money amountIn, QuantLib::Date startDate, QuantLib::Date endDate,
           QuantLib::Calendar calendar, DepositoryAccount* payer, QuantLib::Money closing,
           QuantLib::Rate rate, QuantLib::Money startingBalance, Person& user)
      : RecurringBill(nameIn, amountIn, QuantLib::Period(1, QuantLib::Months), startDate, endDate,
                      calendar, payer, user),
        balance(Account(startingBalance, rate, calendar, nameIn)) {
  }

  ~Mortgage() {
  }

  inline PartiallyDeductiblePayment* getNextBillPayment() {
    QuantLib::Money interest = -1 * balance.getBalance() * balance.getRate() / 12.0;
    QuantLib::Money principal = amount - interest;
    return new PartiallyDeductiblePayment(*paymentIterator, amount, interest);
  }

  Account& getUnderlyingLiability() {
    return balance;
  }
};

#endif  // WORTH_MORTGAGE_H_
