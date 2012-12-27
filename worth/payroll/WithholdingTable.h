/*
 * WithholdingTable.h
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

#ifndef WORTH_PAYROLL_WITHHOLDINGTABLE_H_
#define WORTH_PAYROLL_WITHHOLDINGTABLE_H_


#include <cassert>
#include <vector>
#include <string>
#include <map>
#include "worth/payroll/PayrollPeriods.h"


namespace Worth {
class WithholdingTable {
 private:
  std::map<PayrollFrequency, TieredTaxer*> table;

 public:
  WithholdingTable() {
  }

  ~WithholdingTable() {
    for (std::map<PayrollFrequency, TieredTaxer*>::iterator it = table.begin();
        it != table.end(); ++it) {
      delete (*it).second;
    }
    table.clear();
  }

  QuantLib::Money getTax(PayrollFrequency freq,
                         const QuantLib::Money& taxable) const {
    assert(table.count(freq) > 0);
    std::map<PayrollFrequency, TieredTaxer*>::const_iterator it = table.find(
        freq);
    return it->second->computeTax(taxable);
  }

  void addFrequency(PayrollFrequency freq, TieredTaxer* taxer) {
    table[freq] = taxer;
  }

  bool containsFrequency(PayrollFrequency freq) {
    return table.count(freq) > 0;
  }

  static WithholdingTable* generateWithholdingTable(
      std::vector<std::string> lines, QuantLib::Currency& cur,
      double rateScalingFactor) {
    WithholdingTable* result = new WithholdingTable();
    for (std::vector<std::string>::iterator it = lines.begin();
        it != lines.end(); ++it) {
      boost::char_separator<char> sep(":");
      boost::tokenizer<boost::char_separator<char> > tok(*it, sep);
      boost::tokenizer<boost::char_separator<char> >::iterator tokIt =
          tok.begin();
      std::string freqString = *tokIt;
      assert(tokIt != tok.end());
      ++tokIt;
      std::string tieredTaxerString = *tokIt;

      PayrollFrequency freq;
      if (freqString == "DAILY") {
        freq = Worth::Daily;
      } else if (freqString == "WEEKLY") {
        freq = Worth::Weekly;
      } else if (freqString == "BIWEEKLY") {
        freq = Worth::Biweekly;
      } else if (freqString == "SEMIMONTHLY") {
        freq = Worth::Semimonthly;
      } else if (freqString == "MONTHLY") {
        freq = Worth::Monthly;
      } else if (freqString == "QUARTERLY") {
        freq = Worth::Quarterly;
      } else if (freqString == "SEMIANNUAL") {
        freq = Worth::Semiannual;
      } else if (freqString == "ANNUAL") {
        freq = Worth::Annual;
      }

      TieredTaxer* taxer = TieredTaxer::generateTieredTaxer(tieredTaxerString,
                                                            cur,
                                                            rateScalingFactor);

      result->addFrequency(freq, taxer);
    }

    return result;
  }
};
}
#endif  // WORTH_PAYROLL_WITHHOLDINGTABLE_H_
