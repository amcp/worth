#include "Job.h"
#include <sstream>
using namespace std;

string JobPayment::toString() const {
  stringstream os;

  os << "Date: " << exec << "; Rate: " << hourly << "; Gross: " << grossEarnings
     << "; Income Wages: " << incomeWages << "; SS Wages: " << socialWages
     << "; Deposit: " << amount;

  for (hash_map<string, Money, hash<string> >::const_iterator it = incomeTaxes
      .begin(); it != incomeTaxes.end(); it++) {
    os << "; " << (*it).first << "-SIT: " << (*it).second;
  }

  for (hash_map<string, hash_map<string, Money, hash<string> >, hash<string> >::const_iterator jurisIt =
      socialTaxes.begin(); jurisIt != socialTaxes.end(); jurisIt++) {
    string jurisdiction = (*jurisIt).first;
    for (hash_map<string, Money, hash<string> >::const_iterator taxTypeIt =
        (*jurisIt).second.begin(); taxTypeIt != (*jurisIt).second.end();
        taxTypeIt++) {
      os << "; " << jurisdiction << "-" << (*taxTypeIt).first << ": "
         << (*taxTypeIt).second;
    }
  }

  os << ";";

  return os.str();
}

ostream& operator<<(ostream& os, const JobPayment& pmt) {
  os << pmt.toString();
  return os;
}
