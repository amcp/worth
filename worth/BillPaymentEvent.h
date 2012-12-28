/*
 * RecurringBill.h
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

#ifndef WORTH_BILLPAYMENTEVENT_H_
#define WORTH_BILLPAYMENTEVENT_H_

#include <boost/tokenizer.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include "worth/MyEvent.h"
#include "worth/RecurringBill.h"
#include "worth/DepositoryAccount.h"

namespace Worth {

class BillPaymentEvent : public MyEvent {
 private:
  RecurringBill* bill;
  std::string command;
  std::vector<std::string> tokens;

 public:
  BillPaymentEvent(RecurringBill* billIn, std::string str, QuantLib::Date exec)
      : MyEvent(exec),
        bill(billIn),
        command(str) {
    boost::char_separator<char> sep(", ");
    boost::tokenizer<boost::char_separator<char> > tok(str, sep);
    std::copy(tok.begin, tok.end, tokens.begin());
  }

  ~BillPaymentEvent() {
    tokens.clear();
  }

  std::string toString() const {
    return command;
  }

  void apply(Sequencer* sequencer) {
    if (bill->hasMoreBillPayments()) {
      Payment* payment = bill->getNextBillPayment();
      bill->getPayingAccount()->debitAccount(payment->getAmount());
      sequencer->addEvent(
          new BillPaymentEvent(bill, "PAYMENT", bill->getCurrentPaymentDate()));
      bill->getUser().addBillPayment(bill->getName(), payment);
    }
  }
};
}

#endif  // WORTH_BILLPAYMENTEVENT_H_
