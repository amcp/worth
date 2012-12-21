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

	Money computeTax(Money income) const;

	inline double computeMarginalRate(Money income) {
		double result = 0;
		for(size_t i = 0; i < tiers.size(); i++) {
			if(tiers[i] > income) {
				result = rates[i];
				break;
			}
		}

		return result;
	}

	inline double computeEffectiveRate( Money& income) {
		return computeTax(income) / income;
	}
};

#endif /* TIEREDTAXER_H_ */
