/*
 * PayrollPeriods.h
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
