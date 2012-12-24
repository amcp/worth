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
          statusToCheck = "MARRIED2";
        } else if(status == "MARRIED" && exemptionAllowances <= 1) {
          statusToCheck = "MARRIED1";
        }
      }

      if(income <= state->getLowIncomeExemption(statusToCheck, freq)) {
        return 0 * state->getCurrency();
      }
    }

    QuantLib::Money estimatedDeduction;
    if(state->hasAllowancesInEstimatedDeductionTable(additionalWithholdingAllowances)) {
      estimatedDeduction = state->getEstimatedDeduction(additionalWithholdingAllowances, freq);
    } else {
      estimatedDeduction = state->getEstimatedDeduction(1, freq) * additionalWithholdingAllowances;
    }
    //std::cerr << "Estimated Deduction: " << estimatedDeduction << std::endl;

    QuantLib::Money standardDeduction;
    if(state->getName() == "CA") {
      if(status == "MARRIED") {
        if(exemptionAllowances > 1) {
          standardDeduction = state->getStandardDeduction("MARRIED2", freq);
        } else if(exemptionAllowances <= 1) {
          standardDeduction = state->getStandardDeduction("MARRIED1", freq);
        }
      } else {
        standardDeduction = state->getStandardDeduction(status, freq);
      }
    } else {
      standardDeduction = state->getStandardDeduction(status, freq);
    }
    //std::cerr << "Standard Deduction: " << standardDeduction << std::endl;

    QuantLib::Money taxableIncome = income - standardDeduction - estimatedDeduction;
    //std::cerr << "Taxable Income: " << taxableIncome << std::endl;

    QuantLib::Money computedTax = state->getWithholder(status)->getTax(freq, taxableIncome);
    //std::cerr << "Computed Tax: " << computedTax << std::endl;

    QuantLib::Money exemptionAmount;
    if(state->hasAllowancesInExemptionAllowanceTable(additionalWithholdingAllowances) > 0) {
      exemptionAmount = state->getExemptionAllowance(exemptionAllowances, freq);
    } else {
      exemptionAmount = state->getExemptionAllowance(1, freq) * exemptionAllowances;
    }

    return computedTax - exemptionAmount;
  }
};

} /* namespace Worth */
#endif /* WITHHOLDER_H_ */
