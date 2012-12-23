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
                                     unsigned int additionalWithholdingAllowances) {

    if(state->hasLowIncomeExemptions()) {
      std::string statusToCheck = status;
      if(state->getName() == "CA") {
        if(status == "MARRIED" && exemptionAllowances > 1) {
          statusToCheck = "HEADOFHOUSEHOLD";
        } else if(status == "MARRIED" && exemptionAllowances <= 1) {
          statusToCheck = "SINGLE";
        }
      }

      if(income <= state->getLowIncomeExemption(statusToCheck, freq)) {
        return 0 * state->getCurrency();
      }
    }

    QuantLib::Money estimatedDeduction = state->getEstimatedDeduction(freq) * additionalWithholdingAllowances;
    QuantLib::Money standardDeduction = state->getStandardDeduction(status, freq);
    if(state->getName() == "CA") {
      if(status == "MARRIED" && exemptionAllowances > 1) {
        standardDeduction = state->getStandardDeduction("HEADOFHOUSEHOLD", freq);
      } else if(status == "MARRIED" && exemptionAllowances <= 1) {
        standardDeduction = state->getStandardDeduction("SINGLE", freq);
      }
    }
    QuantLib::Money taxableIncome = income - standardDeduction - estimatedDeduction;
    QuantLib::Money computedTax = state->getWithholder(status)->getTax(freq, taxableIncome);
    QuantLib::Money exemptionAmount = state->getExemptionAllowance(freq) * exemptionAllowances;

    return computedTax - exemptionAmount;
  }
};

} /* namespace Worth */
#endif /* WITHHOLDER_H_ */
