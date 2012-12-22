/*
 * Witholder.h
 *
 *  Created on: Dec 21, 2012
 *      Author: amcp
 */

#ifndef WITHOLDER_H_
#define WITHOLDER_H_

#include <cassert>
#include "payroll/PayrollPeriods.h"

namespace Worth {
class Withholder {
 private:
  std::map<PayrollFrequency, TieredTaxer*> table;
 public:
  Withholder() {}

  ~Withholder() {
    table.clear();
  }

  QuantLib::Money getTax(PayrollFrequency freq, const QuantLib::Money& taxable) {
    assert(table.count(freq) > 0);
    return table[freq]->computeTax(taxable);
  }

  void addFrequency(PayrollFrequency freq, TieredTaxer* taxer) {
    table[freq] = taxer;
  }

  bool containsFrequency(PayrollFrequency freq) {
    return table.count(freq) > 0;
  }
};
}
#endif /* WITHOLDER_H_ */
