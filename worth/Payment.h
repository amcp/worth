/*
 * Payment.h
 *
 *  Created on: 23 √ÎÙ 2012
 *      Copyright 2012 Alexander Patrikalakis
 */

#ifndef WORTH_PAYMENT_H_
#define WORTH_PAYMENT_H_

#include <ql/time/date.hpp>
#include <ql/money.hpp>

class Payment {
 protected:
  QuantLib::Date exec;
  QuantLib::Money amount;
  Payment() {
  }

 public:
  Payment(QuantLib::Date d, QuantLib::Money notional)
      : exec(d),
        amount(notional) {
  }
  ~Payment() {
  }
  inline QuantLib::Money getAmount() const {
    return amount;
  }
  inline QuantLib::Date getDate() const {
    return exec;
  }
};

class PartiallyDeductiblePayment : public Payment {
 protected:
  QuantLib::Money deductiblePortion;
  PartiallyDeductiblePayment()
      : Payment() {
  }

 public:
  PartiallyDeductiblePayment(QuantLib::Date d, QuantLib::Money notional,
                             QuantLib::Money deduct)
      : Payment(d, notional),
        deductiblePortion(deduct) {
  }

  inline QuantLib::Money getDeductiblePortion() const {
    return deductiblePortion;
  }
};

#endif  // WORTH_PAYMENT_H_
