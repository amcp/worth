#include "Job.h"
#include <cmath>
#include <ql/cashflows/simplecashflow.hpp>
#include "Account.h"
#include "tax/TieredTaxer.h"
#include "tax/TaxDictionary.h"

Job::~Job() {
}

Job::Job(Person* userIn, Money hourlyRate, double raiseRate, Calendar calendar,
         Money preIncomeTaxDeductions, Money preSocialTaxDeductions,
         Money taxableDeductions,
         hash_map<string, Rate, hash<string> > taxingJurisdictions,
         hash_map<string, int> ex, int hours, Period freq, Date startDate,
         Date endDate, int accumVacHrs, DepositoryAccount* dd,
         DepositoryAccount* employerContributionAccount,
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

void Job::setPreIncomeTaxDeductionsPerPeriod(Money deduct) {
  QL_REQUIRE(deduct.currency() == balance.currency(),
             "Deductions must be made in the right currency.");
  this->preIncomeTaxDeductionsPerPeriod = deduct;
}

void Job::setPreSocialTaxDeductionsPerPeriod(Money deduct) {
  QL_REQUIRE(deduct.currency() == balance.currency(),
             "Deductions must be made in the right currency.");
  this->preSocialTaxDeductionsPerPeriod = deduct;
}

void Job::setTaxableDeductionsPerPeriod(Money deduct) {
  QL_REQUIRE(deduct.currency() == balance.currency(),
             "Deductions must be made in the right currency.");
  this->taxableDeductionsPerPeriod = deduct;
}

JobPayment* Job::getNextPayment() {
  Money imputedVacIncome = vacationHoursTakenInPayPeriod * getHourlyRate();
  Money payrollAdjSocialIncome = getAdjustedAnnualSocialIncome(
      extraPayInPeriod);
  Money payrollAdjIncome = getAdjustedAnnualIncome(extraPayInPeriod);
  Worth::TaxDictionary* dict = Worth::TaxDictionary::getInstance();
  unsigned int year = getCurrentPaymentDate().year();

  hash_map<string, Money, hash<string> > stateIncomeTaxes;
  for (hash_map<string, int, hash<string> >::const_iterator it = exemptions
      .begin(); it != exemptions.end(); it++) {
    if ((*it).first.compare("US") != 0) {
      //for states, compute income tax
      hash_map<string, Rate, hash<string> >::const_iterator jurisIt =
          taxJurisdictions.find((*it).first);
      Rate jurisRate = (*jurisIt).second;
      Money exemptionAmt = dict->getExemptionAmount(year, (*it).first);
      hash_map<string, int, hash<string> >::const_iterator exemptionIt =
          exemptions.find((*it).first);
      unsigned int noExemptions = (*exemptionIt).second;
      Money payrollAdjIncomeMinusExemption = payrollAdjIncome
          - exemptionAmt * noExemptions;
      stateIncomeTaxes[(*it).first] = dict->getIncomeTaxer(year, (*it).first)
          ->computeTax(payrollAdjIncomeMinusExemption);
      stateIncomeTaxes[(*it).first] *= jurisRate;
      stateIncomeTaxes[(*it).first] /=
          static_cast<double>(getPayPeriodsPerYear());
    }
  }

  //subtract no exempt. * exempt notional and subtract state income taxes paid
  Money stateIt = 0 * balance.currency();
  for (hash_map<string, Money, hash<string> >::iterator it = stateIncomeTaxes
      .begin(); it != stateIncomeTaxes.end(); it++) {
    stateIt += (*it).second;
  }
  Money federalExemption = dict->getExemptionAmount(year, "US");
  hash_map<string, int, hash<string> >::const_iterator exemptionIt = exemptions
      .find("US");
  int federalNoExemptions = (*exemptionIt).second;
  Money federalPayrollAdjIncome = payrollAdjIncome
      - federalExemption * federalNoExemptions - stateIt;  // + imputedVacIncome;

  stateIncomeTaxes["US"] = dict->getIncomeTaxer(year, "US")->computeTax(
      federalPayrollAdjIncome) / static_cast<double>(getPayPeriodsPerYear());

  Money socialTaxes = 0 * balance.currency();
  hash_map<string, hash_map<string, Money, hash<string> >, hash<string> > socialTaxMap;
  for (hash_map<string, Rate, hash<string> >::const_iterator it =
      taxJurisdictions.begin(); it != taxJurisdictions.end(); it++) {
    if (!dict->hasSocialTaxers(year, (*it).first)) {
      continue;
    }
    hash_map<string, Worth::TieredTaxer*, hash<string> > taxers =
        dict->getSocialTaxers(year, (*it).first);
    for (hash_map<string, Worth::TieredTaxer*, hash<string> >::const_iterator jurisTaxerIt =
        taxers.begin(); jurisTaxerIt != taxers.end(); jurisTaxerIt++) {
      hash_map<string, Rate, hash<string> >::const_iterator jurisIt =
          taxJurisdictions.find((*it).first);
      Rate jurisRate = (*jurisIt).second;
      hash_map<string, Worth::TieredTaxer*, hash<string> >::const_iterator taxerIt =
          taxers.find((*jurisTaxerIt).first);
      Money proratedSocialTax = jurisRate
          * (*taxerIt).second->computeTax(payrollAdjSocialIncome)
          / static_cast<double>(getPayPeriodsPerYear());
      socialTaxMap[(*it).first][(*jurisTaxerIt).first] = proratedSocialTax;
      socialTaxes += proratedSocialTax;
    }
  }

  Money payrollAdjIncomePerPeriod = payrollAdjIncome
      / static_cast<double>(getPayPeriodsPerYear());
  Money periodPayAfterTaxation = payrollAdjIncomePerPeriod
      - stateIncomeTaxes["US"] - stateIt - socialTaxes
      - getTaxableDeductionsPerPeriod();
  Money gross = balance * double(effort)
      / static_cast<double>(getPayPeriodsPerYear()) + extraPayInPeriod;
  Money employerContribution = gross * this->employerRetirementGiftRatio;
  employerContribution = employerContribution
      + min(
          this->preIncomeTaxDeductionsPerPeriod
              * this->employerRetirementMatchRatio,
          gross * this->employerRetirementMatchMaximum);

  JobPayment* result = new JobPayment(
      periodPayAfterTaxation, getCurrentPaymentDate(), gross,
      payrollAdjIncomePerPeriod,
      payrollAdjSocialIncome / static_cast<double>(getPayPeriodsPerYear()),
      stateIncomeTaxes, socialTaxMap, getHourlyRate(),
      this->preIncomeTaxDeductionsPerPeriod, employerContribution);

  paymentIterator++;
  return result;
}
