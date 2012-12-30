/*
 * Withholder.h
 *
 * Created on: Dec 21, 2012
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

#ifndef WORTH_PAYROLL_WITHHOLDER_H_
#define WORTH_PAYROLL_WITHHOLDER_H_

#include <ql/money.hpp>
#include <cstdio>
#include <string>
#include "worth/payroll/PayrollPeriods.h"
#include "worth/jurisdiction/State.h"
#include "worth/Utility.h"

namespace Worth {

class Withholder {
 private:
  State* state;

 public:
  explicit Withholder(State* stateIn)
      : state(stateIn) {
  }
  ~Withholder() {
    delete state;
  }

  QuantLib::Money computeWithholding(
      QuantLib::Money incomeIn, PayrollFrequency freqIn, FilingStatus status,
      unsigned int exemptionAllowances,
      unsigned int additionalWithholdingAllowances,
      unsigned int withholdingAllowances, bool annualWithholding = false) {
    QuantLib::Money income = incomeIn;
    PayrollFrequency freq = freqIn;
    if (annualWithholding) {
      freq = Worth::Annual;
      if (freqIn == Worth::Semiannual) {
        income *= 2;
      } else if (freqIn == Worth::Quarterly) {
        income *= 4;
      } else if (freqIn == Worth::Monthly) {
        income *= 12;
      } else if (freqIn == Worth::Semimonthly) {
        income *= 24;
      } else if (freqIn == Worth::Biweekly) {
        income *= 26;
      } else if (freqIn == Worth::Weekly) {
        income *= 52;
      } else if (freqIn == Worth::Daily) {
        income *= 259;
      }
    }

    Utility* util = Utility::getInstance();

    printf("State: %s; Filing Status: %s;\n", state->getName().c_str(),
           status.c_str());
    printf("Payroll Frequency: %d; Period wages: %s;\n", freq,
           util->convertMoney(income).c_str());
    printf("Exemptions: %u; Additional withholding allowances: %u;\n",
           exemptionAllowances, additionalWithholdingAllowances);

    std::string statusToCheck = status;
    if (state->getName() == "CA") {
      if (status == "MARRIED" && exemptionAllowances > 1) {
        statusToCheck = "MARRIED2";
      } else if (status == "MARRIED" && exemptionAllowances <= 1) {
        statusToCheck = "MARRIED1";
      }
    }

    if (state->hasLowIncomeExemptionForStatus(statusToCheck)
        && income <= state->getLowIncomeExemption(statusToCheck, freq)) {
      return 0 * state->getCurrency();
    }

    QuantLib::Money estimatedDeduction(0, income.currency());
    if (state->hasAllowancesInEstimatedDeductionTable(
        additionalWithholdingAllowances)) {
      estimatedDeduction = state->getEstimatedDeduction(
          additionalWithholdingAllowances, freq);
    } else if (state->hasAllowancesInEstimatedDeductionTable(1)) {
      estimatedDeduction = state->getEstimatedDeduction(1, freq)
          * additionalWithholdingAllowances;
    }
    printf("Estimated Deduction: %s\n",
           util->convertMoney(estimatedDeduction).c_str());

    QuantLib::Money withholdingAllowance(0, income.currency());
    if (state->hasAllowancesInWithholdingAllowanceTable(
        withholdingAllowances)) {
      withholdingAllowance = state->getWithholdingAllowance(
          withholdingAllowances, freq);
    } else if (state->hasAllowancesInWithholdingAllowanceTable(1)) {
      withholdingAllowance = state->getWithholdingAllowance(1, freq)
          * withholdingAllowances;
    }
    printf("Withholding allowance: %s\n",
           util->convertMoney(withholdingAllowance).c_str());

    QuantLib::Money standardDeduction(0, income.currency());
    if (state->getName() == "CA") {
      if (status == "MARRIED") {
        if (exemptionAllowances > 1) {
          standardDeduction = state->getStandardDeduction("MARRIED2", freq);
        } else if (exemptionAllowances <= 1) {
          standardDeduction = state->getStandardDeduction("MARRIED1", freq);
        }
      } else {
        standardDeduction = state->getStandardDeduction(status, freq);
      }
    } else if (state->hasStandardDeductionForStatus(status)) {
      standardDeduction = state->getStandardDeduction(status, freq);
    }
    printf("Standard Deduction: %s\n", util->convertMoney(standardDeduction).c_str());

    QuantLib::Money taxableIncome = income - standardDeduction
        - estimatedDeduction - withholdingAllowance;
    printf("Taxable Income: %s\n", util->convertMoney(taxableIncome).c_str());

    QuantLib::Money computedTax = state->getWithholder(status)->getTax(
        freq, taxableIncome);
    printf("Computed Tax: %s\n", util->convertMoney(computedTax).c_str());

    QuantLib::Money exemptionAmount(0, income.currency());
    if (state->hasAllowancesInExemptionAllowanceTable(
        additionalWithholdingAllowances)) {
      exemptionAmount = state->getExemptionAllowance(exemptionAllowances, freq);
    } else if (state->hasAllowancesInExemptionAllowanceTable(1)) {
      exemptionAmount = state->getExemptionAllowance(1, freq)
          * exemptionAllowances;
    }

    QuantLib::Money taxWithheld = computedTax - exemptionAmount;

    if (annualWithholding) {
      printf("Tax Withheld (annualized): %s\n",
             util->convertMoney(taxWithheld).c_str());
      freq = Worth::Annual;
      if (freqIn == Worth::Semiannual) {
        taxWithheld /= 2;
      } else if (freqIn == Worth::Quarterly) {
        taxWithheld /= 4;
      } else if (freqIn == Worth::Monthly) {
        taxWithheld /= 12;
      } else if (freqIn == Worth::Semimonthly) {
        taxWithheld /= 24;
      } else if (freqIn == Worth::Biweekly) {
        taxWithheld /= 26;
      } else if (freqIn == Worth::Weekly) {
        taxWithheld /= 52;
      } else if (freqIn == Worth::Daily) {
        taxWithheld /= 259;
      }
    }
    printf("Tax Withheld per period: %s\n\n",
           util->convertMoney(taxWithheld).c_str());

    return taxWithheld;
  }
};

} /* namespace Worth */
#endif  // WORTH_PAYROLL_WITHHOLDER_H_
