/*
 * State.h
 *
 *  Created on: Dec 21, 2012
 *      Author: amcp
 */

#ifndef STATE_H_
#define STATE_H_
#include <cassert>
#include <ql/money.hpp>
#include "../payroll/Withholder.h"
#include "../payroll/PayrollPeriods.h"

namespace Worth {

typedef std::string FilingStatus;
typedef unsigned int Year;

class State {
 private:
  std::string jurisdictionName;
  Year year;
  QuantLib::Currency currency;
  std::set<FilingStatus> filingStatuses;
  std::map<FilingStatus, const WithholdingTable* > withholdingTables;
  std::map<PayrollFrequency, QuantLib::Money > exemptionAllowanceTable;
  std::map<FilingStatus, std::map<PayrollFrequency, QuantLib::Money > > lowIncomeExemptionTable;
  std::map<FilingStatus, std::map<PayrollFrequency, QuantLib::Money > > standardDeductionTable;

 public:
  State(std::string name, Year yearIn, QuantLib::Currency cur) : jurisdictionName(name), year(yearIn), currency(cur) { }

  const std::string& getName() { return jurisdictionName; }
  inline Year getYear() { return year; }

  const WithholdingTable* getWithholder(const FilingStatus& status) const {
    //assert(filingStatuses.count(status) > 0);
    assert(withholdingTables.count(status) > 0);

    std::map<FilingStatus, const WithholdingTable* >::const_iterator it = withholdingTables.find(status);
    return it->second;
  }

  void addWithholder(const FilingStatus& status, const WithholdingTable* table) {
    withholdingTables[status] = table;
  }

  const QuantLib::Money& getExemptionAllowance(PayrollFrequency freq) const {
    assert(exemptionAllowanceTable.count(freq) > 0);
    std::map<PayrollFrequency, QuantLib::Money >::const_iterator it = exemptionAllowanceTable.find(freq);
    return it->second;
  }

  void addExemptionAllowance(PayrollFrequency freq, const QuantLib::Money& allowance) {
    exemptionAllowanceTable[freq] = allowance;
  }

  const QuantLib::Currency& getCurrency() const {
    return currency;
  }

  bool hasLowIncomeExemptions() const {
    return lowIncomeExemptionTable.size() > 0;
  }

  const QuantLib::Money& getLowIncomeExemption(const FilingStatus& status, PayrollFrequency freq) const {
    //assert(filingStatuses.count(status) > 0);
    assert(lowIncomeExemptionTable.count(status) > 0);
    std::map<FilingStatus, std::map<PayrollFrequency, QuantLib::Money > >::const_iterator itToFreqExMap;
    itToFreqExMap = lowIncomeExemptionTable.find(status);
    assert(itToFreqExMap->second.count(freq) > 0);
    std::map<PayrollFrequency, QuantLib::Money >::const_iterator itToExMap;
    itToExMap = itToFreqExMap->second.find(freq);
    return itToExMap->second;
  }

  void addLowIncomeExemption(const FilingStatus& status, PayrollFrequency freq, QuantLib::Money mon) {
    lowIncomeExemptionTable[status][freq] = mon;
  }

  const QuantLib::Money& getStandardDeduction(const FilingStatus& status, PayrollFrequency freq) const {
    //assert(filingStatuses.count(status) > 0);
    assert(standardDeductionTable.count(status) > 0);
    std::map<FilingStatus, std::map<PayrollFrequency, QuantLib::Money > >::const_iterator itToFreqExMap;
    itToFreqExMap = standardDeductionTable.find(status);
    assert(itToFreqExMap->second.count(freq) > 0);
    std::map<PayrollFrequency, QuantLib::Money >::const_iterator itToExMap;
    itToExMap = itToFreqExMap->second.find(freq);
    return itToExMap->second;
  }

  void addStandardDeduction(const FilingStatus& status, PayrollFrequency freq, QuantLib::Money mon) {
    standardDeductionTable[status][freq] = mon;
  }

  ~State() {
    filingStatuses.clear();
    withholdingTables.clear();
    exemptionAllowanceTable.clear();
  }
};

} /* namespace Worth */
#endif /* STATE_H_ */
