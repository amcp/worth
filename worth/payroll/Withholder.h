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
  State* state;
 public:
  explicit Withholder(State* stateIn) : state(stateIn) {

  }
  ~Withholder() {
    delete state;
  }

  QuantLib::Money computeWithholding(QuantLib::Money income,
                                     PayrollFrequency freq,
                                     FilingStatus status,
                                     unsigned int exemptionAllowances,
                                     unsigned int additionalWithholdingExemptions) {
    if(state->hasLowIncomeExemptions() && income <= state->getLowIncomeExemption(status, freq)) {
      return 0 * state->getCurrency();
    }

    QuantLib::Money estimatedDeduction = state->getEstimatedDeduction(freq) * additionalWithholdingExemptions;
    QuantLib::Money taxableIncome = income - state->getStandardDeduction(status, freq) - estimatedDeduction;
    QuantLib::Money computedTax = state->getWithholder(status)->getTax(freq, taxableIncome);
    QuantLib::Money exemptionAmount = state->getExemptionAllowance(freq) * exemptionAllowances;

    return computedTax - exemptionAmount;
  }
};

} /* namespace Worth */
#endif /* WITHHOLDER_H_ */
