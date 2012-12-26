/*
 * TieredTaxer.h
 *
 *  Created on: Oct 31, 2011
 *   Copyright 2012 Alexander Patrikalakis
 */

#ifndef WORTH_TAX_TIEREDTAXER_H_
#define WORTH_TAX_TIEREDTAXER_H_

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <ql/money.hpp>
#include <ql/currency.hpp>

#include <cassert>
#include <map>
#include <vector>
#include <string>

namespace Worth {
class TieredTaxer {
 private:
  QuantLib::Currency currency;
  std::vector<QuantLib::Money> tiers;
  std::vector<double> rates;

 public:
  explicit TieredTaxer(const QuantLib::Currency& curr)
      : currency(curr) {
  }
  ~TieredTaxer() {
    tiers.clear();
    rates.clear();
  }

  inline void addTier(const QuantLib::Money threshold, double rate) {
    QL_REQUIRE(threshold.currency() == currency,
               "Tried to add tax tier in different currency.");
    tiers.push_back(threshold);
    rates.push_back(rate);
  }

  QuantLib::Money computeTax(const QuantLib::Money& income) const {
    QuantLib::Money total(income.currency(), 0);

    std::stringstream msg;
    msg << "Income " << income << " was not denominated in " << this->currency;
    QL_REQUIRE(income.currency() != this->currency, msg.str());

    QuantLib::Money workingRemainder = income;

    for (int i = tiers.size() - 1; i >= 0; i--) {
      if (workingRemainder < tiers[i]) {
        // go to the next one if the current tier is
        // greater than the working remainder
        continue;
      }

      const QuantLib::Money tier = tiers.at(i);
      QuantLib::Money delta = workingRemainder - tier;
      QuantLib::Rate rate = rates[i];
      total = total + delta * rate;
      workingRemainder = tiers[i];
    }

    return total;
  }

  inline double computeMarginalRate(const QuantLib::Money& income) const {
    double rate = 0.0;
    for (unsigned int i = 0; i < tiers.size(); i++) {
      if (income < tiers[i]) {
        break;
      }
      rate = rates[i];
    }

    return rate;
  }

  inline double computeEffectiveRate(const QuantLib::Money& income) {
    return income > 0 * currency ? computeTax(income) / income : 0.0;
  }

  static TieredTaxer* generateTieredTaxer(const std::string& str,
                                          QuantLib::Currency& cur,
                                          double rateScalingFactor = 1.0) {
    TieredTaxer* result = new TieredTaxer(cur);

    // get tokens in str
    boost::char_separator<char> sep(", ");
    boost::tokenizer<boost::char_separator<char> > tok(str, sep);

    for (boost::tokenizer<boost::char_separator<char> >::iterator beg = tok
        .begin(); beg != tok.end(); ++beg) {
      std::string moneyString = *beg;
      assert(beg != tok.end());
      ++beg;
      std::string rateString = *beg;
      result->addTier(
          boost::lexical_cast<double>(moneyString) * cur,
          boost::lexical_cast<double>(rateString) * rateScalingFactor);
    }

    return result;
  }
};
}

#endif  // WORTH_TAX_TIEREDTAXER_H_
