/*
 * PayrollPeriods.h
 *
 *  Created on: Dec 21, 2012
 *   Copyright 2012 Alexander Patrikalakis
 */

#ifndef WORTH_PAYROLL_PAYROLLPERIODS_H_
#define WORTH_PAYROLL_PAYROLLPERIODS_H_

#include <boost/algorithm/string.hpp>
#include <string>

namespace Worth {
enum PayrollFrequency {
  Daily = 0,
  Weekly,
  Biweekly,
  Semimonthly,
  Monthly,
  Quarterly,
  Semiannual,
  Annual,
  Unknown
};

static PayrollFrequency convertStringToPayrollFrequency(const std::string& in) {
  std::string str = boost::to_upper_copy(in);
  PayrollFrequency freq;
  if (str == "DAILY") {
    freq = Worth::Daily;
  } else if (str == "WEEKLY") {
    freq = Worth::Weekly;
  } else if (str == "BIWEEKLY") {
    freq = Worth::Biweekly;
  } else if (str == "SEMIMONTHLY") {
    freq = Worth::Semimonthly;
  } else if (str == "MONTHLY") {
    freq = Worth::Monthly;
  } else if (str == "QUARTERLY") {
    freq = Worth::Quarterly;
  } else if (str == "SEMIANNUAL") {
    freq = Worth::Semiannual;
  } else if (str == "ANNUAL") {
    freq = Worth::Annual;
  } else {  // dummy value for ALLOWANCES
    freq = Worth::Unknown;
  }

  return freq;
}
}

#endif  // WORTH_PAYROLL_PAYROLLPERIODS_H_
