/*
 * Person.cpp
 *
 *  Created on: 23 √ÎÙ 2012
 *      Author: amcp
 */

#include "Job.h"
#include "tax/TaxDictionary.h"

Person::Person(Currency cur, hash_map<string, int> ex)
    : currency(cur),
      nominalExemptions(ex),
      mainDepository(NULL) {

}

Person::~Person() {
  assets.clear();
  liabilities.clear();
}

Money Person::getNetWorth() {
  Money result(0, currency);

  for (vector<DepositoryAccount*>::iterator it = assets.begin();
      it != assets.end(); it++) {
    result += (*it)->getBalance();
  }

  for (vector<DepositoryAccount*>::iterator it = liabilities.begin();
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

hash_map<string, Money, hash<string> > Person::generateTaxReturn(
    int year, Money deductionsInAllJurisdictions) {
  cout << "Processing " << year
       << " tax return. Deductions in all jurisdictions are: "
       << deductionsInAllJurisdictions << endl;
  hash_map<string, Money, hash<string> > result;
  hash_map<string, Money, hash<string> > taxesPaid;
  hash_map<string, Money, hash<string> > taxesOwed;
  set<string> stateJurisdictions;
  Money yearlyIncomeWages = 0 * currency;

  if (paymentsPerCalendarYearAndMonth.count(year) == 0) {
    return result;
  }

  Money taxesPaidToStateJurisdictions = 0 * currency;
  for (hash_map<int, vector<JobPayment*> >::iterator monthIt =
      paymentsPerCalendarYearAndMonth[year].begin();
      monthIt != paymentsPerCalendarYearAndMonth[year].end(); monthIt++) {
    for (vector<JobPayment*>::iterator it = (*monthIt).second.begin();
        it != (*monthIt).second.end(); it++) {
      JobPayment* pmt = *it;

      hash_map<string, Money, hash<string> > paymentIncomeTaxesPaid = pmt
          ->getIncomeTaxesPaid();
      for (hash_map<string, Money, hash<string> >::iterator incomeIt =
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

      hash_map<string, hash_map<string, Money, hash<string> >, hash<string> > paymentSocialTaxesPaid =
          pmt->getSocialTaxesPaid();
      hash_map<string, hash_map<string, Money, hash<string> >, hash<string> >::iterator socialIt;
      for (socialIt = paymentSocialTaxesPaid.begin();
          socialIt != paymentSocialTaxesPaid.end(); socialIt++) {
        if ((*socialIt).first.compare("US") != 0) {
          hash_map<string, Money, hash<string> > jurisSocialTaxMap = (*socialIt)
              .second;
          for (hash_map<string, Money, hash<string> >::iterator socialTypeIt =
              jurisSocialTaxMap.begin();
              socialTypeIt != jurisSocialTaxMap.end(); socialTypeIt++) {
            taxesPaidToStateJurisdictions += (*socialTypeIt).second;  //for federal income tax purposes.
          }
        }
      }

      yearlyIncomeWages += pmt->getIncomeWages();
    }
  }

  cout << "Tax paid to state jurisdictions for federal income tax purposes in "
       << year << ": " << taxesPaidToStateJurisdictions << endl;
  cout << "Yearly income wages were: " << yearlyIncomeWages << endl;

  TaxDictionary* dict = TaxDictionary::getInstance();
  Money federalExemptions = dict->getExemptionAmount(year, "US");
  Money federalTaxableIncome = yearlyIncomeWages - taxesPaidToStateJurisdictions
      - deductionsInAllJurisdictions - federalExemptions;
  cout << "US Taxable Income for " << year << ": " << federalTaxableIncome
       << " (AGI was " << yearlyIncomeWages << ", exemption "
       << federalExemptions << ")" << endl;
  taxesOwed["US"] = dict->getIncomeTaxer(year, "US")->computeTax(
      federalTaxableIncome);
  for (set<string>::iterator it = stateJurisdictions.begin();
      it != stateJurisdictions.end(); it++) {
    Money taxableIncome = yearlyIncomeWages - deductionsInAllJurisdictions
        - dict->getExemptionAmount(year, *it);
    cout << *it << " Taxable Income for " << year << ": " << taxableIncome
         << " (AGI was " << yearlyIncomeWages << ", exemption "
         << dict->getExemptionAmount(year, *it) << ")" << endl;
    taxesOwed[*it] = dict->getIncomeTaxer(year, *it)->computeTax(taxableIncome);
  }

  //TODO rehash with days-in-year ratios and jurisdiction source income
  cout << "Federal Taxable Income for " << year << ": " << yearlyIncomeWages
       << " (AGI was " << yearlyIncomeWages << ")" << endl;
  for (hash_map<string, Money, hash<string> >::iterator it = taxesPaid.begin();
      it != taxesPaid.end(); it++) {
    result[(*it).first] = (*it).second - taxesOwed[(*it).first];
    cout << (*it).first << " taxes withheld: " << (*it).second << "; "
         << (*it).first << " taxes actually owed: " << taxesOwed[(*it).first]
         << endl;
  }

  cout << endl;

  generateYearEndSummary(year);

  return result;
}
