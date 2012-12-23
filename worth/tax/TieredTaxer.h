/*
 * TieredTaxer.h
 *
 *  Created on: Oct 31, 2011
 *      Author: amcp
 */

#ifndef TIEREDTAXER_H_
#define TIEREDTAXER_H_

#include <iostream>
#include <map>
#include <vector>
#include <cassert>
#include <ql/money.hpp>
#include <ql/currency.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace QuantLib;

namespace Worth {
class TieredTaxer {
private:
	Currency currency;
	vector<Money> tiers;
	vector<double> rates;

public:
	TieredTaxer(Currency& curr) : currency(curr) {}
	~TieredTaxer() {
		tiers.clear();
		rates.clear();
	}

	inline void addTier(const Money threshold, double rate) {
		QL_REQUIRE(threshold.currency() == currency, "Tried to add tax tier in different currency.");
		tiers.push_back(threshold);
		rates.push_back(rate);
	}

	Money computeTax(const Money& income) const {
    Money total(income.currency(), 0);

    if(income.currency() != this->currency) {
      cerr << "Income " << income << " was not denominated in " << this->currency << endl;
      return total;
    }

    Money workingRemainder = income;

    for (int i = tiers.size() - 1; i >= 0; i--) {
      if(workingRemainder < tiers[i]) {
        //go to the next one if the current tier is greater than the working remainder
        continue;
      }

      const Money tier = tiers.at(i);
      Money delta = workingRemainder - tier;
      Rate rate = rates[i];
      total = total + delta * rate;
      workingRemainder = tiers[i];
    }

    return total;
  }

	inline double computeMarginalRate(const Money& income) const {
	  double rate = 0.0;
	  for (unsigned int i = 0; i < tiers.size(); i++) {
	    if(income < tiers[i]) {
        break;
      }
	    rate = rates[i];
	  }

	  return rate;
	}

	inline double computeEffectiveRate(const Money& income) {
		return income > 0 * currency ? computeTax(income) / income : 0.0;
	}

	static TieredTaxer* generateTieredTaxer(const std::string& str, Currency& cur) {
	  TieredTaxer* result = new TieredTaxer(cur);

	  //get tokens in str
	  boost::char_separator<char> sep(", ");
	  boost::tokenizer<boost::char_separator<char> > tok(str, sep);

	  for(boost::tokenizer<boost::char_separator<char> >::iterator beg = tok.begin(); beg != tok.end(); ++beg) {
	    std::string rateString = *beg;
	    assert(beg != tok.end());
	    ++beg;
	    std::string moneyString = *beg;
	    result->addTier(boost::lexical_cast<double>(moneyString) * cur, boost::lexical_cast<double>(rateString));
	  }

	  return result;
	}
};
}

#endif /* TIEREDTAXER_H_ */
