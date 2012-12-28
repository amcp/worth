/**
 * JobPayment.cpp
 *
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

#include <ext/hash_map>
#include <sstream>
#include <string>

#include "worth/Job.h"
#include "worth/Utility.h"

namespace Worth {

std::string JobPayment::toString() const {
  Utility* util = Utility::getInstance();
  std::stringstream os;

  os << "Date: " << util->convertDate(exec) << "; Rate: "
     << util->convertMoney(hourly) << "; Gross: "
     << util->convertMoney(grossEarnings) << "; Income Wages: "
     << util->convertMoney(incomeWages) << "; SS Wages: "
     << util->convertMoney(socialWages) << "; Deposit: "
     << util->convertMoney(amount);

  JobPayment::StringMoneyMap::const_iterator it;
  for (it = incomeTaxes.begin(); it != incomeTaxes.end(); it++) {
    os << "; " << (*it).first << "-SIT: " << (*it).second;
  }
  __gnu_cxx ::hash_map<std::string, JobPayment::StringMoneyMap,
      __gnu_cxx ::hash<std::string> >::const_iterator jurisIt;

  for (jurisIt = socialTaxes.begin(); jurisIt != socialTaxes.end(); jurisIt++) {
    std::string jurisdiction = (*jurisIt).first;
    JobPayment::StringMoneyMap::const_iterator taxTypeIt;
    for (taxTypeIt = (*jurisIt).second.begin();
        taxTypeIt != (*jurisIt).second.end(); taxTypeIt++) {
      os << "; " << jurisdiction << "-" << (*taxTypeIt).first << ": "
         << (*taxTypeIt).second;
    }
  }

  os << ";";

  return os.str();
}
}
