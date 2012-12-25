/*
 * State.h
 *
 *  Created on: Dec 21, 2012
 *      Author: amcp
 */

#ifndef STATE_H_
#define STATE_H_
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <ql/money.hpp>
#include <ql/currency.hpp>
#include "../payroll/Withholder.h"
#include "../payroll/PayrollPeriods.h"

namespace Worth {

typedef std::string FilingStatus;
typedef unsigned int Year;

class State {
 public:
  typedef std::map<PayrollFrequency, QuantLib::Money> FrequencyTable;
  typedef std::map<unsigned int, FrequencyTable> AllowanceFrequencyTable;
  typedef std::map<FilingStatus, FrequencyTable> StatusFrequencyTable;

 private:
  std::string jurisdictionName;
  Year year;
  QuantLib::Currency currency;
  std::map<FilingStatus, WithholdingTable*> withholdingTables;
  AllowanceFrequencyTable exemptionAllowanceTable;
  AllowanceFrequencyTable estimatedAllowanceTable;
  AllowanceFrequencyTable withholdingAllowanceTable;
  StatusFrequencyTable lowIncomeExemptionTable;
  StatusFrequencyTable standardDeductionTable;

 public:
  State(std::string name, Year yearIn, QuantLib::Currency cur)
      : jurisdictionName(name),
        year(yearIn),
        currency(cur) {
  }
  ~State() {
    for (std::map<FilingStatus, WithholdingTable*>::iterator it =
        withholdingTables.begin(); it != withholdingTables.end(); ++it) {
      delete it->second;
    }
    withholdingTables.clear();
    exemptionAllowanceTable.clear();
    estimatedAllowanceTable.clear();
    lowIncomeExemptionTable.clear();
    standardDeductionTable.clear();
  }

  const std::string& getName() {
    return jurisdictionName;
  }
  inline Year getYear() {
    return year;
  }

  const WithholdingTable* getWithholder(const FilingStatus& status) const {
    //assert(filingStatuses.count(status) > 0);
    assert(withholdingTables.count(status) > 0);

    std::map<FilingStatus, WithholdingTable*>::const_iterator it =
        withholdingTables.find(status);
    return it->second;
  }

  void addWithholder(const FilingStatus& status, WithholdingTable* table) {
    withholdingTables[status] = table;
  }

  const QuantLib::Money& getExemptionAllowance(unsigned int allowances,
                                               PayrollFrequency freq) const {
    assert(exemptionAllowanceTable.count(allowances) > 0);
    AllowanceFrequencyTable::const_iterator it = exemptionAllowanceTable.find(
        allowances);
    assert(it->second.count(freq) > 0);
    FrequencyTable::const_iterator allowanceIt = it->second.find(freq);
    return allowanceIt->second;
  }

  void addExemptionAllowance(unsigned int allowances, PayrollFrequency freq,
                             const QuantLib::Money& allowanceValue) {
    exemptionAllowanceTable[allowances][freq] = allowanceValue;
  }

  void addExemptionAllowances(AllowanceFrequencyTable::const_iterator first, AllowanceFrequencyTable::const_iterator last) {
    exemptionAllowanceTable.insert(first, last);
  }

  bool hasAllowancesInExemptionAllowanceTable(unsigned int allowances) {
    return exemptionAllowanceTable.count(allowances) > 0;
  }

  const QuantLib::Money& getEstimatedDeduction(unsigned int allowances,
                                               PayrollFrequency freq) const {
    assert(estimatedAllowanceTable.count(allowances) > 0);
    AllowanceFrequencyTable::const_iterator it = estimatedAllowanceTable.find(
        allowances);
    assert(it->second.count(freq) > 0);
    FrequencyTable::const_iterator allowanceIt = it->second.find(freq);
    return allowanceIt->second;
  }

  void addEstimatedDeduction(unsigned int allowances, PayrollFrequency freq,
                             const QuantLib::Money& allowanceValue) {
    estimatedAllowanceTable[allowances][freq] = allowanceValue;
  }

  void addEstimatedDeductions(AllowanceFrequencyTable::const_iterator first, AllowanceFrequencyTable::const_iterator last) {
    estimatedAllowanceTable.insert(first, last);
  }

  bool hasAllowancesInEstimatedDeductionTable(unsigned int allowances) {
    return estimatedAllowanceTable.count(allowances) > 0;
  }

  const QuantLib::Money& getWithholdingAllowance(unsigned int allowances,
                                               PayrollFrequency freq) const {
    assert(withholdingAllowanceTable.count(allowances) > 0);
    AllowanceFrequencyTable::const_iterator it = withholdingAllowanceTable.find(
        allowances);
    assert(it->second.count(freq) > 0);
    FrequencyTable::const_iterator allowanceIt = it->second.find(freq);
    return allowanceIt->second;
  }

  void addWithholdingAllowance(unsigned int allowances, PayrollFrequency freq,
                             const QuantLib::Money& allowanceValue) {
    withholdingAllowanceTable[allowances][freq] = allowanceValue;
  }

  void addWithholdingAllowances(AllowanceFrequencyTable::const_iterator first, AllowanceFrequencyTable::const_iterator last) {
    withholdingAllowanceTable.insert(first, last);
  }

  bool hasAllowancesInWithholdingAllowanceTable(unsigned int allowances) {
    return withholdingAllowanceTable.count(allowances) > 0;
  }

  const QuantLib::Currency& getCurrency() const {
    return currency;
  }

  bool hasLowIncomeExemptionForStatus(const FilingStatus& status) const {
    return lowIncomeExemptionTable.count(status) > 0;
  }

  const QuantLib::Money& getLowIncomeExemption(const FilingStatus& status,
                                               PayrollFrequency freq) const {
    assert(lowIncomeExemptionTable.count(status) > 0);
    StatusFrequencyTable::const_iterator itToFreqExMap;
    itToFreqExMap = lowIncomeExemptionTable.find(status);
    assert(itToFreqExMap->second.count(freq) > 0);
    FrequencyTable::const_iterator itToExMap;
    itToExMap = itToFreqExMap->second.find(freq);
    return itToExMap->second;
  }

  void addLowIncomeExemption(const FilingStatus& status, PayrollFrequency freq,
                             QuantLib::Money mon) {
    lowIncomeExemptionTable[status][freq] = mon;
  }

  void addLowIncomeExemptions(StatusFrequencyTable::const_iterator begin, StatusFrequencyTable::const_iterator end) {
    lowIncomeExemptionTable.insert(begin, end);
  }

  const QuantLib::Money& getStandardDeduction(const FilingStatus& status,
                                              PayrollFrequency freq) const {
    assert(hasStandardDeductionForStatus(status));
    StatusFrequencyTable::const_iterator itToFreqExMap;
    itToFreqExMap = standardDeductionTable.find(status);
    assert(itToFreqExMap->second.count(freq) > 0);
    FrequencyTable::const_iterator itToExMap;
    itToExMap = itToFreqExMap->second.find(freq);
    return itToExMap->second;
  }

  bool hasStandardDeductionForStatus(const FilingStatus& status) const {
    return standardDeductionTable.count(status) > 0;
  }

  void addStandardDeduction(const FilingStatus& status, PayrollFrequency freq,
                            QuantLib::Money mon) {
    standardDeductionTable[status][freq] = mon;
  }

  void addStandardDeductions(StatusFrequencyTable::const_iterator begin, StatusFrequencyTable::const_iterator end) {
    standardDeductionTable.insert(begin, end);
  }

  static AllowanceFrequencyTable generateAllowanceFrequencyTable(
      std::vector<std::string> lines, QuantLib::Currency& cur) {
    //first line is column titles
    assert(lines.size() > 0);

    std::vector<std::string>::iterator lineIt = lines.begin();

    boost::char_separator<char> sep(" :");
    boost::tokenizer<boost::char_separator<char> > tok(*lineIt, sep);
    std::vector<std::string> frequencyStrings(tok.begin(), tok.end());
    std::vector<PayrollFrequency> frequencies(frequencyStrings.size());
    std::transform(frequencyStrings.begin(), frequencyStrings.end(), frequencies.begin(), Worth::convertStringToPayrollFrequency);

    ++lineIt;
    AllowanceFrequencyTable result;
    while (lineIt != lines.end()) {
      boost::tokenizer<boost::char_separator<char> > tok(*lineIt, sep);
      std::vector<std::string> tokens(tok.begin(), tok.end());
      std::for_each(tokens.begin(), tokens.end(), boost::bind(&boost::trim<std::string>, _1, std::locale() ));

      assert(tokens.size() == frequencies.size());
      unsigned int nAllowances = boost::lexical_cast<unsigned int>(tokens[0]);
      for (size_t i = 1; i < frequencies.size(); i++) {
        std::string token = tokens[i];
        double amt = boost::lexical_cast<double>(token);
        result[nAllowances][frequencies[i]] = amt * cur;
      }
      ++lineIt;
    }

    return result;
  }

  static StatusFrequencyTable generateStatusFrequencyTable(std::vector<std::string> lines, QuantLib::Currency& cur) {
    //first line is filing status names titles
    assert(lines.size() > 0);

    std::vector<std::string>::iterator lineIt = lines.begin();

    boost::char_separator<char> sep(" ");
    boost::tokenizer<boost::char_separator<char> > tok(*lineIt, sep);
    std::vector<std::string> statuses(tok.begin(), tok.end());

    ++lineIt;
    StatusFrequencyTable result;
    while (lineIt != lines.end()) {
      boost::tokenizer<boost::char_separator<char> > tok(*lineIt, sep);
      std::vector<std::string> tokens(tok.begin(), tok.end());
      std::for_each(tokens.begin(), tokens.end(), boost::bind(&boost::trim<std::string>, _1, std::locale() ));

      assert(tokens.size() == statuses.size());
      Worth::PayrollFrequency freq = Worth::convertStringToPayrollFrequency(tokens[0]);
      for (size_t i = 1; i < statuses.size(); i++) {
        result[statuses[i]][freq] = boost::lexical_cast<double>(tokens[i]) * cur;
      }
      ++lineIt;
    }

    return result;
  }
};

} /* namespace Worth */
#endif /* STATE_H_ */
