/*
 * Withholder.h
 *
 *  Created on: Dec 21, 2012
 *      Author: amcp
 */

#ifndef WITHHOLDER_H_
#define WITHHOLDER_H_

#include <ql/money.hpp>
#include "PayrollPeriods.h"
#include "../jurisdiction/State.h"

namespace Worth {

class Withholder {
 private:
  const State& state;
 public:
  explicit Withholder(const State& stateIn) : state(stateIn) {

  }
  ~Withholder() {}

  QuantLib::Money computeWithholding(QuantLib::Money income,
                                     PayrollFrequency freq,
                                     FilingStatus status,
                                     unsigned int withholdingExemptions) {
if(state.hasLowIncomeExemptions() && income <= state.getLowIncomeExemption(status, freq)) {
      return 0 * state.getCurrency();
    }

    QuantLib::Money taxableIncome = income - state.getStandardDeduction(status, freq);
    QuantLib::Money computedTax = state.getWithholder(status)->getTax(freq, taxableIncome);
    QuantLib::Money exemptionAmount = state.getExemptionAllowance(freq) * withholdingExemptions;

    return computedTax - exemptionAmount;
  }
};

} /* namespace Worth */
#endif /* WITHHOLDER_H_ */
