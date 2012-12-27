/*
 * Payment.h
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
