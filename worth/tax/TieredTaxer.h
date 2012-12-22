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
#include <ql/money.hpp>
#include <ql/currency.hpp>

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

	inline void addTier(double threshold, double rate) {
		tiers.push_back(threshold * currency);
		rates.push_back(rate);
	}

	Money computeTax(const Money& income) const;

	inline double computeMarginalRate(const Money& income) {
	  double rate = 0.0;
	  for (unsigned int i = tiers.size() - 1; i >= 0; i--) {
	    if(income < tiers[i]) {
        //go to the next one if the current tier is greater than the working remainder
        break;
      }
	    rate = rates[i];
	  }

	  return rate;
	}

	inline double computeEffectiveRate(const Money& income) {
		return income > 0 * currency ? computeTax(income) / income : 0.0;
	}
};
}

#endif /* TIEREDTAXER_H_ */
