/*
 * Person.cpp
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

#include "worth/Job.h"

#include <vector>
#include <set>
#include <string>
#include <ext/hash_map>

#include "worth/tax/TaxDictionary.h"

Person::Person(QuantLib::Currency cur,
               __gnu_cxx ::hash_map<std::string, int> ex)
    : currency(cur),
      nominalExemptions(ex),
      mainDepository(NULL) {
}

Person::~Person() {
  assets.clear();
  liabilities.clear();
}

QuantLib::Money Person::getNetWorth() {
  QuantLib::Money result(0, currency);

  for (std::vector<DepositoryAccount*>::iterator it = assets.begin();
      it != assets.end(); it++) {
    result += (*it)->getBalance();
  }

  for (std::vector<DepositoryAccount*>::iterator it = liabilities.begin();
      it != liabilities.end(); it++) {
    result += (*it)->getBalance();
  }

  return result;
}

void Person::addAsset(DepositoryAccount* asset) {
  assets.push_back(asset);
  if (asset->getType() == CASH && mainDepository == NULL) {
    mainDepository = asset;
  }
}

JobPayment::StringMoneyMap Person::generateTaxReturn(
    int year, QuantLib::Money deductionsInAllJurisdictions) {
  std::cout << "Processing " << year
            << " tax return. Deductions in all jurisdictions are: "
            << deductionsInAllJurisdictions << std::endl;
  JobPayment::StringMoneyMap result;
  JobPayment::StringMoneyMap taxesPaid;
  JobPayment::StringMoneyMap taxesOwed;
  std::set<std::string> stateJurisdictions;
  QuantLib::Money yearlyIncomeWages = 0 * currency;

  if (paymentsPerCalendarYearAndMonth.count(year) == 0) {
    return result;
  }

  QuantLib::Money taxesPaidToStateJurisdictions = 0 * currency;
  for (__gnu_cxx ::hash_map<int, std::vector<JobPayment*> >::iterator monthIt =
      paymentsPerCalendarYearAndMonth[year].begin();
      monthIt != paymentsPerCalendarYearAndMonth[year].end(); monthIt++) {
    for (std::vector<JobPayment*>::iterator it = (*monthIt).second.begin();
        it != (*monthIt).second.end(); it++) {
      JobPayment* pmt = *it;

      JobPayment::StringMoneyMap paymentIncomeTaxesPaid = pmt
          ->getIncomeTaxesPaid();
      for (JobPayment::StringMoneyMap::iterator incomeIt =
          paymentIncomeTaxesPaid.begin();
          incomeIt != paymentIncomeTaxesPaid.end(); incomeIt++) {
        if (taxesPaid.count((*incomeIt).first) == 0) {
          taxesPaid[(*incomeIt).first] = 0 * currency;
        }

        taxesPaid[(*incomeIt).first] += (*incomeIt).second;
        if ((*incomeIt).first.compare("US") != 0) {
          taxesPaidToStateJurisdictions += (*incomeIt).second;
          if (stateJurisdictions.count((*incomeIt).first) == 0) {
            stateJurisdictions.insert((*incomeIt).first);
          }
        }
      }

      __gnu_cxx ::hash_map<std::string, JobPayment::StringMoneyMap,
          __gnu_cxx ::hash<std::string> > paymentSocialTaxesPaid = pmt
          ->getSocialTaxesPaid();
      __gnu_cxx ::hash_map<std::string, JobPayment::StringMoneyMap,
          __gnu_cxx ::hash<std::string> >::iterator socialIt;
      for (socialIt = paymentSocialTaxesPaid.begin();
          socialIt != paymentSocialTaxesPaid.end(); socialIt++) {
        if ((*socialIt).first.compare("US") != 0) {
          JobPayment::StringMoneyMap jurisSocialTaxMap = (*socialIt).second;
          for (JobPayment::StringMoneyMap::iterator socialTypeIt =
              jurisSocialTaxMap.begin();
              socialTypeIt != jurisSocialTaxMap.end(); socialTypeIt++) {
            // for federal income tax purposes.
            taxesPaidToStateJurisdictions += (*socialTypeIt).second;
          }
        }
      }

      yearlyIncomeWages += pmt->getIncomeWages();
    }
  }

  std::cout
      << "Tax paid to state jurisdictions for federal income tax purposes in "
      << year << ": " << taxesPaidToStateJurisdictions << std::endl;
  std::cout << "Yearly income wages were: " << yearlyIncomeWages << std::endl;

  // TODO(amcp) fix use of tax dictionary
  Worth::TaxDictionary* dict = NULL;  // Worth::TaxDictionary::getInstance();
  QuantLib::Money federalExemptions = dict->getExemptionAmount(year, "US");
  QuantLib::Money federalTaxableIncome = yearlyIncomeWages
      - taxesPaidToStateJurisdictions - deductionsInAllJurisdictions
      - federalExemptions;
  std::cout << "US Taxable Income for " << year << ": " << federalTaxableIncome
            << " (AGI was " << yearlyIncomeWages << ", exemption "
            << federalExemptions << ")" << std::endl;
  taxesOwed["US"] = dict->getIncomeTaxer(year, "US")->computeTax(
      federalTaxableIncome);
  for (std::set<std::string>::iterator itin = stateJurisdictions.begin();
      itin != stateJurisdictions.end(); itin++) {
    QuantLib::Money taxableIncome = yearlyIncomeWages
        - deductionsInAllJurisdictions - dict->getExemptionAmount(year, *itin);
    std::cout << *itin << " Taxable Income for " << year << ": "
              << taxableIncome << " (AGI was " << yearlyIncomeWages
              << ", exemption " << dict->getExemptionAmount(year, *itin) << ")"
              << std::endl;
    taxesOwed[*itin] = dict->getIncomeTaxer(year, *itin)->computeTax(
        taxableIncome);
  }

  // TODO(amcp) rehash with days-in-year ratios and jurisdiction source income
  std::cout << "Federal Taxable Income for " << year << ": "
            << yearlyIncomeWages << " (AGI was " << yearlyIncomeWages << ")"
            << std::endl;
  for (JobPayment::StringMoneyMap::iterator itin = taxesPaid.begin();
      itin != taxesPaid.end(); itin++) {
    result[(*itin).first] = (*itin).second - taxesOwed[(*itin).first];
    std::cout << (*itin).first << " taxes withheld: " << (*itin).second << "; "
              << (*itin).first << " taxes actually owed: "
              << taxesOwed[(*itin).first] << std::endl;
  }

  std::cout << std::endl;

  generateYearEndSummary(year);

  return result;
}
