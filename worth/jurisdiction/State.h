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
#include "payroll/Withholder.h"
#include "payroll/PayrollPeriods.h"

namespace Worth {

typedef std::string FilingStatus;
typedef unsigned int Year;

class State {
 private:
  std::string jurisdictionName;
  Year year;
  std::set<FilingStatus> filingStatuses;
  std::map<FilingStatus, const TieredTaxer& > incomeTaxers;
  std::map<FilingStatus, const TieredTaxer& > socialTaxers;
  std::map<FilingStatus, const Withholder& > withholders;
  std::map<PayrollFrequency, QuantLib::Money > exemptionAllowanceTable;

 public:
  State(std::string name, Year yearIn) : jurisdictionName(name), year(yearIn) { }

  const std::string& getName() { return jurisdictionName; }
  inline Year getYear() { return year; }

  const Withholder& getWithholder(const FilingStatus& status) {
    assert(filingStatuses.count(status) > 0);
    assert(withholders.count(status) > 0);
    return withholders[status];
  }

  ~State() {
    filingStatuses.clear();
    incomeTaxers.clear();
    socialTaxers.clear();
    withholders.clear();
    exemptionAllowanceTable.clear();
  }
};

} /* namespace Worth */
#endif /* STATE_H_ */
