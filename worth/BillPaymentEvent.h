/*
 * RecurringBill.h
 *
 *  Created on: 23 √ÎÙ 2012
 *   Copyright 2012 Alexander Patrikalakis
 */

#ifndef WORTH_BILLPAYMENTEVENT_H_
#define WORTH_BILLPAYMENTEVENT_H_

#include <boost/tokenizer.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include "worth/MyEvent.h"
#include "worth/RecurringBill.h"

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

  string toString() const {
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

#endif  // WORTH_BILLPAYMENTEVENT_H_
