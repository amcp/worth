/**
 * Copyright 2012 Alexander Patrikalakis
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
