/*
 * Payment.hh
 *
 *  Created on: 23 √ÎÙ 2012
 *      Author: amcp
 */

#ifndef PAYMENT_H_
#define PAYMENT_H_

#include <ql/time/date.hpp>
#include <ql/money.hpp>

using namespace QuantLib;

class Payment {
 protected:
  Date exec;
  Money amount;
  Payment() {
  }
 public:
  Payment(Date d, Money notional)
      : exec(d),
        amount(notional) {
  }
  ~Payment() {
  }
  inline Money getAmount() const {
    return amount;
  }
  inline Date getDate() const {
    return exec;
  }
};

class PartiallyDeductiblePayment : public Payment {
 protected:
  Money deductiblePortion;
  PartiallyDeductiblePayment()
      : Payment() {
  }
 public:
  PartiallyDeductiblePayment(Date d, Money notional, Money deduct)
      : Payment(d, notional),
        deductiblePortion(deduct) {

  }

  inline Money getDeductiblePortion() const {
    return deductiblePortion;
  }
};

#endif /* PAYMENT_H_ */
