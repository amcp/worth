/*
 * WithholdingTable.h
 *
 *  Created on: Dec 21, 2012
 *      Author: amcp
 */

#ifndef WITHHOLDINGTABLE_H_
#define WITHHOLDINGTABLE_H_

#include <cassert>
#include "PayrollPeriods.h"

namespace Worth {
class WithholdingTable {
 private:
  std::map<PayrollFrequency, const TieredTaxer*> table;
 public:
  WithholdingTable() {}

  ~WithholdingTable() {
    table.clear();
  }

  QuantLib::Money getTax(PayrollFrequency freq, const QuantLib::Money& taxable) const {
    assert(table.count(freq) > 0);
    std::map<PayrollFrequency, const TieredTaxer*>::const_iterator it = table.find(freq);
    return it->second->computeTax(taxable);
    //return table[freq]->computeTax(taxable);
  }

  void addFrequency(PayrollFrequency freq, const TieredTaxer* taxer) {
    table[freq] = taxer;
  }

  bool containsFrequency(PayrollFrequency freq) {
    return table.count(freq) > 0;
  }
};
}
#endif /* WITHHOLDINGTABLE_H_ */
