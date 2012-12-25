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

  QuantLib::Money computeWithholding(QuantLib::Money incomeIn,
                                     PayrollFrequency freqIn,
                                     FilingStatus status,
                                     unsigned int exemptionAllowances,
                                     unsigned int additionalWithholdingAllowances,
                                     unsigned int withholdingAllowances,
                                     bool annualWithholding = false) {
    QuantLib::Money income = incomeIn;
    PayrollFrequency freq = freqIn;
    if(annualWithholding) {
      freq = Worth::Annual;
      if(freqIn == Worth::Semiannual) {
        income *= 2;
      } else if(freqIn == Worth::Quarterly) {
        income *= 4;
      } else if(freqIn == Worth::Monthly) {
        income *= 12;
      } else if(freqIn == Worth::Semimonthly) {
        income *= 24;
      } else if(freqIn == Worth::Biweekly) {
        income *= 26;
      } else if (freqIn == Worth::Weekly) {
        income *= 52;
      } else if(freqIn == Worth::Daily) {
        income *= 259;
      }
    }

    std::cerr << "State: " << state->getName() << "; Filing Status: " << status << ";" << std::endl;
    std::cerr << "Payroll Frequency: " << freq << "; Period wages: " << income << ";" << std:: endl;
    std::cerr << "Exemptions: " << exemptionAllowances
        << "; Additional withholding allowances: " << additionalWithholdingAllowances << std::endl;


    std::string statusToCheck = status;
    if(state->getName() == "CA") {
      if(status == "MARRIED" && exemptionAllowances > 1) {
        statusToCheck = "MARRIED2";
      } else if(status == "MARRIED" && exemptionAllowances <= 1) {
        statusToCheck = "MARRIED1";
      }
    }

    if(state->hasLowIncomeExemptionForStatus(statusToCheck) && income <= state->getLowIncomeExemption(statusToCheck, freq)) {
      return 0 * state->getCurrency();
    }

    QuantLib::Money estimatedDeduction(0, income.currency());
    if(state->hasAllowancesInEstimatedDeductionTable(additionalWithholdingAllowances)) {
      estimatedDeduction = state->getEstimatedDeduction(additionalWithholdingAllowances, freq);
    } else if(state->hasAllowancesInEstimatedDeductionTable(1)) {
      estimatedDeduction = state->getEstimatedDeduction(1, freq) * additionalWithholdingAllowances;
    }
    std::cerr << "Estimated Deduction: " << estimatedDeduction << std::endl;

    QuantLib::Money withholdingAllowance(0, income.currency());
    if(state->hasAllowancesInWithholdingAllowanceTable(withholdingAllowances)) {
      withholdingAllowance = state->getWithholdingAllowance(withholdingAllowances, freq);
    } else if(state->hasAllowancesInWithholdingAllowanceTable(1)) {
      withholdingAllowance = state->getWithholdingAllowance(1, freq) * withholdingAllowances;
    }
    std::cerr << "Withholding allowance: " << withholdingAllowance << std::endl;

    QuantLib::Money standardDeduction(0, income.currency());
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
    } else if(state->hasStandardDeductionForStatus(status)){
      standardDeduction = state->getStandardDeduction(status, freq);
    }
    std::cerr << "Standard Deduction: " << standardDeduction << std::endl;

    QuantLib::Money taxableIncome = income - standardDeduction - estimatedDeduction - withholdingAllowance;
    std::cerr << "Taxable Income: " << taxableIncome << std::endl;

    QuantLib::Money computedTax = state->getWithholder(status)->getTax(freq, taxableIncome);
    std::cerr << "Computed Tax: " << computedTax << std::endl;

    QuantLib::Money exemptionAmount(0, income.currency());
    if(state->hasAllowancesInExemptionAllowanceTable(additionalWithholdingAllowances)) {
      exemptionAmount = state->getExemptionAllowance(exemptionAllowances, freq);
    } else if(state->hasAllowancesInExemptionAllowanceTable(1)){
      exemptionAmount = state->getExemptionAllowance(1, freq) * exemptionAllowances;
    }

    QuantLib::Money taxWithheld = computedTax - exemptionAmount;

    if(annualWithholding) {
      std::cerr << "Tax Withheld (annualized): " << taxWithheld << std::endl;
      freq = Worth::Annual;
      if(freqIn == Worth::Semiannual) {
        taxWithheld /= 2;
      } else if(freqIn == Worth::Quarterly) {
        taxWithheld /= 4;
      } else if(freqIn == Worth::Monthly) {
        taxWithheld /= 12;
      } else if(freqIn == Worth::Semimonthly) {
        taxWithheld /= 24;
      } else if(freqIn == Worth::Biweekly) {
        taxWithheld /= 26;
      } else if (freqIn == Worth::Weekly) {
        taxWithheld /= 52;
      } else if(freqIn == Worth::Daily) {
        taxWithheld /= 259;
      }
    }
    std::cerr << "Tax Withheld per period: " << taxWithheld << std::endl << std::endl;

    return taxWithheld;
  }
};

} /* namespace Worth */
#endif /* WITHHOLDER_H_ */
