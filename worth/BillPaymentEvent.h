/*
 * RecurringBill.h
 *
 *  Created on: 23 √ÎÙ 2012
 *      Author: amcp
 */

#ifndef BILLPAYMENTEVENT_H_
#define BILLPAYMENTEVENT_H_

#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>
#include "MyEvent.h"
#include "RecurringBill.h"

using namespace std;
using namespace QuantLib;

class BillPaymentEvent : public MyEvent {
 private:
  RecurringBill& bill;
  string command;
  vector<string> tokens;
 public:
  BillPaymentEvent(RecurringBill& billIn, string str, Date exec)
      : MyEvent(exec),
        bill(billIn),
        command(str) {
    std::stringstream strstr(str);
    std::istream_iterator<std::string> it(strstr);
    std::istream_iterator<std::string> end;
    tokens.insert(tokens.begin(), it, end);
  }

  ~BillPaymentEvent() {
    tokens.clear();
  }

  string toString() const {
    return command;
  }

  void apply(Sequencer& sequencer) {
    if (bill.hasMoreBillPayments()) {
      Payment* payment = bill.getNextBillPayment();
      bill.getPayingAccount()->debitAccount(payment->getAmount());
      sequencer.addEvent(
          new BillPaymentEvent(bill, "PAYMENT", bill.getCurrentPaymentDate()));
      bill.getUser().addBillPayment(bill.getName(), payment);
    }
  }
};

#endif /* BILLPAYMENTEVENT_H_ */
