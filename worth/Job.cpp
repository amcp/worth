/*
 * Job.cpp
 *
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

#include "worth/Job.h"
#include <ql/money.hpp>
#include <ql/cashflows/simplecashflow.hpp>
#include <map>
#include <cmath>
#include <string>
#include "worth/DepositoryAccount.h"
#include "worth/tax/TieredTaxer.h"
#include "worth/tax/TaxDictionary.h"

namespace Worth {

Job::~Job() {
}

Job::Job(Person* userIn, QuantLib::Money hourlyRate, double raiseRate,
         QuantLib::Calendar calendar, QuantLib::Money preIncomeTaxDeductions,
         QuantLib::Money preSocialTaxDeductions,
         QuantLib::Money taxableDeductions,
         std::map<std::string, QuantLib::Rate> taxingJurisdictions,
         std::map<std::string, int> ex, int hours, QuantLib::Period freq,
         QuantLib::Date startDate, QuantLib::Date endDate, int accumVacHrs,
         DepositoryAccount* dd, DepositoryAccount* employerContributionAccount,
         DepositoryAccount* employeeContributionAccount,
         double erReturementGift, double erRetirementMatchMaximum,
         double erRetirementMatchRatio)
    : user(userIn),
      balance(hourlyRate),
      preIncomeTaxDeductionsPerPeriod(preIncomeTaxDeductions),
      preSocialTaxDeductionsPerPeriod(preSocialTaxDeductions),
      taxableDeductionsPerPeriod(taxableDeductions),
      taxJurisdictions(taxingJurisdictions),
      exemptions(ex),
      effort(hours),
      payPeriod(freq),
      start(startDate),
      end(endDate),
      accumulatedVacationHours(accumVacHrs),
      vacationHoursTakenInPayPeriod(0),
      extraPayInPeriod(0 * hourlyRate.currency()),
      ddAccount(dd),
      employerRetirementContributionAccount(employerContributionAccount),
      employeeRetirementContributionAccount(employeeContributionAccount),
      employerRetirementGiftRatio(erReturementGift),
      employerRetirementMatchMaximum(erRetirementMatchMaximum),
      employerRetirementMatchRatio(erRetirementMatchRatio) {
  paySchedule = Worth::Utility::getInstance()->generateSchedule(this->payPeriod,
                                                                start, end,
                                                                calendar);
  paymentIterator = paySchedule.begin();
}

void Job::setPreIncomeTaxDeductionsPerPeriod(QuantLib::Money deduct) {
  QL_REQUIRE(deduct.currency() == balance.currency(),
             "Deductions must be made in the right currency.");
  this->preIncomeTaxDeductionsPerPeriod = deduct;
}

void Job::setPreSocialTaxDeductionsPerPeriod(QuantLib::Money deduct) {
  QL_REQUIRE(deduct.currency() == balance.currency(),
             "Deductions must be made in the right currency.");
  this->preSocialTaxDeductionsPerPeriod = deduct;
}

void Job::setTaxableDeductionsPerPeriod(QuantLib::Money deduct) {
  QL_REQUIRE(deduct.currency() == balance.currency(),
             "Deductions must be made in the right currency.");
  this->taxableDeductionsPerPeriod = deduct;
}

JobPayment* Job::getNextPayment() {
  QuantLib::Money imputedVacIncome = vacationHoursTakenInPayPeriod
      * getHourlyRate();
  QuantLib::Money payrollAdjSocialIncome = getAdjustedAnnualSocialIncome(
      extraPayInPeriod);
  QuantLib::Money payrollAdjIncome = getAdjustedAnnualIncome(extraPayInPeriod);
  // TODO(amcp)
  Worth::TaxDictionary* dict = NULL;  // Worth::TaxDictionary::getInstance();
  unsigned int year = getCurrentPaymentDate().year();

  std::map<std::string, QuantLib::Money> stateIncomeTaxes;
  for (NominalExemptionMap::const_iterator it = exemptions.begin();
      it != exemptions.end(); it++) {
    if ((*it).first.compare("US") != 0) {
      // for states, compute income tax
      std::map<std::string, QuantLib::Rate>::const_iterator jurisIt =
          taxJurisdictions.find((*it).first);
      QuantLib::Rate jurisRate = (*jurisIt).second;
      QuantLib::Money exemptionAmt = dict->getExemptionAmount(year,
                                                              (*it).first);
      NominalExemptionMap::const_iterator exemptionIt = exemptions.find(
          (*it).first);
      unsigned int noExemptions = (*exemptionIt).second;
      QuantLib::Money payrollAdjIncomeMinusExemption = payrollAdjIncome
          - exemptionAmt * noExemptions;
      stateIncomeTaxes[(*it).first] = dict->getIncomeTaxer(year, (*it).first)
          ->computeTax(payrollAdjIncomeMinusExemption);
      stateIncomeTaxes[(*it).first] *= jurisRate;
      stateIncomeTaxes[(*it).first] /=
          static_cast<double>(getPayPeriodsPerYear());
    }
  }

  // subtract no exempt. * exempt notional and subtract state income taxes paid
  QuantLib::Money stateIt = 0 * balance.currency();
  for (std::map<std::string, QuantLib::Money>::iterator it = stateIncomeTaxes
      .begin(); it != stateIncomeTaxes.end(); it++) {
    stateIt += (*it).second;
  }
  QuantLib::Money federalExemption = dict->getExemptionAmount(year, "US");
  NominalExemptionMap::const_iterator exemptionIt = exemptions.find("US");
  int federalNoExemptions = (*exemptionIt).second;
  QuantLib::Money federalPayrollAdjIncome = payrollAdjIncome;
  federalPayrollAdjIncome -= stateIt;
  federalPayrollAdjIncome -= federalExemption * federalNoExemptions;
  // + imputedVacIncome;

  stateIncomeTaxes["US"] = dict->getIncomeTaxer(year, "US")->computeTax(
      federalPayrollAdjIncome) / static_cast<double>(getPayPeriodsPerYear());

  QuantLib::Money socialTaxes = 0 * balance.currency();
  std::map<std::string, JobPayment::StringMoneyMap> socialTaxMap;
  std::map<std::string, QuantLib::Rate>::const_iterator it;
  for (it = taxJurisdictions.begin(); it != taxJurisdictions.end(); it++) {
    if (!dict->hasSocialTaxers(year, (*it).first)) {
      continue;
    }
    std::map<std::string, Worth::TieredTaxer*> taxers = dict->getSocialTaxers(
        year, (*it).first);
    std::map<std::string, Worth::TieredTaxer*>::const_iterator jurisTaxerIt;
    for (jurisTaxerIt = taxers.begin(); jurisTaxerIt != taxers.end();
        jurisTaxerIt++) {
      std::map<std::string, QuantLib::Rate>::const_iterator jurisIt =
          taxJurisdictions.find((*it).first);
      QuantLib::Rate jurisRate = (*jurisIt).second;
      std::map<std::string, Worth::TieredTaxer*>::const_iterator taxerIt =
          taxers.find((*jurisTaxerIt).first);
      QuantLib::Money proratedSocialTax = jurisRate
          * (*taxerIt).second->computeTax(payrollAdjSocialIncome)
          / static_cast<double>(getPayPeriodsPerYear());
      socialTaxMap[(*it).first][(*jurisTaxerIt).first] = proratedSocialTax;
      socialTaxes += proratedSocialTax;
    }
  }

  QuantLib::Money payrollAdjIncomePerPeriod = payrollAdjIncome
      / static_cast<double>(getPayPeriodsPerYear());
  QuantLib::Money periodPayAfterTaxation = payrollAdjIncomePerPeriod
      - stateIncomeTaxes["US"] - stateIt - socialTaxes
      - getTaxableDeductionsPerPeriod();
  QuantLib::Money gross = balance * static_cast<double>(effort)
      / static_cast<double>(getPayPeriodsPerYear()) + extraPayInPeriod;
  QuantLib::Money employerContribution = gross
      * this->employerRetirementGiftRatio;
  QuantLib::Money employerMatch = preIncomeTaxDeductionsPerPeriod
      * employerRetirementMatchRatio;
  QuantLib::Money employerMaxMatch = gross * employerRetirementMatchMaximum;
  if (employerMatch < employerMaxMatch) {
    employerContribution += employerMatch;
  } else {
    employerContribution += employerMaxMatch;
  }

  JobPayment* result = new JobPayment(
      periodPayAfterTaxation, getCurrentPaymentDate(), gross,
      payrollAdjIncomePerPeriod,
      payrollAdjSocialIncome / static_cast<double>(getPayPeriodsPerYear()),
      stateIncomeTaxes, socialTaxMap, getHourlyRate(),
      this->preIncomeTaxDeductionsPerPeriod, employerContribution);

  paymentIterator++;
  return result;
}
}
