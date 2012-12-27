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
#include "Job.h"
#include <sstream>
#include <string>
#include <ext/hash_map>

std::string JobPayment::toString() const {
  std::stringstream os;

  os << "Date: " << exec << "; Rate: " << hourly << "; Gross: " << grossEarnings
     << "; Income Wages: " << incomeWages << "; SS Wages: " << socialWages
     << "; Deposit: " << amount;
  __gnu_cxx::hash_map<std::string, QuantLib::Money, __gnu_cxx::hash<std::string> >::const_iterator it;
  for (it = incomeTaxes.begin(); it != incomeTaxes.end(); it++) {
    os << "; " << (*it).first << "-SIT: " << (*it).second;
  }
  __gnu_cxx::hash_map<std::string, __gnu_cxx::hash_map<std::string, QuantLib::Money, __gnu_cxx::hash<std::string> >, __gnu_cxx::hash<std::string> >::const_iterator jurisIt;

  for (jurisIt = socialTaxes.begin(); jurisIt != socialTaxes.end(); jurisIt++) {
    std::string jurisdiction = (*jurisIt).first;
    __gnu_cxx::hash_map<std::string, QuantLib::Money, __gnu_cxx::hash<std::string> >::const_iterator taxTypeIt;
    for (taxTypeIt = (*jurisIt).second.begin(); taxTypeIt != (*jurisIt).second.end();
        taxTypeIt++) {
      os << "; " << jurisdiction << "-" << (*taxTypeIt).first << ": "
         << (*taxTypeIt).second;
    }
  }

  os << ";";

  return os.str();
}

std::ostream& operator<<(std::ostream& os, const JobPayment& pmt) {
  os << pmt.toString();
  return os;
}
