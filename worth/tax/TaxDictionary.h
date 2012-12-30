/*
 * TaxDictionary.h
 *
 * Created on: 22 √ÎÙ 2012
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

#ifndef WORTH_TAX_TAXDICTIONARY_H_
#define WORTH_TAX_TAXDICTIONARY_H_

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <ql/money.hpp>
#include <ql/currencies/america.hpp>

#include <map>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "worth/tax/TieredTaxer.h"
#include "worth/Utility.h"

namespace Worth {
typedef std::map<std::string, QuantLib::Money> ExemptionMap;
typedef std::map<std::string, TieredTaxer*> IncomeTaxerMap;
typedef std::map<std::string, IncomeTaxerMap> SocialTaxerMap;

class TaxDictionary {
 private:
  TaxDictionary() {
  }
  std::map<int, ExemptionMap> exemptionAmtPerJuris;
  std::map<int, IncomeTaxerMap> incomeTaxers;
  std::map<int, SocialTaxerMap> socialTaxers;
  QuantLib::Currency currency;

  TaxDictionary(std::string fname, QuantLib::Currency currencyIn)
      : currency(currencyIn) {
    Utility* util = Utility::getInstance();

    std::vector<std::string> lines = util->readLines(fname);
    for (std::vector<std::string>::iterator lineIt = lines.begin();
        lineIt != lines.end(); ++lineIt) {
      boost::char_separator<char> sep(", ");
      boost::tokenizer<boost::char_separator<char> > tok(*lineIt, sep);
      std::vector<std::string> tokens;
      std::copy(tok.begin(), tok.end(), tokens.begin());

      if (tokens.size() == 0) {
        continue;
      }

      // process record
      unsigned int year = boost::lexical_cast<unsigned int>(tokens[0]);
      std::string type = tokens[1];
      std::string jurisdiction = tokens[2];
      if (type.compare("INCOME") == 0) {
        if (incomeTaxers[year].count(jurisdiction) == 0) {
          incomeTaxers[year][jurisdiction] = new TieredTaxer(currency);
        }
        incomeTaxers[year][jurisdiction]->addTier(
            boost::lexical_cast<double>(tokens[3]) * currency,
            boost::lexical_cast<double>(tokens[4]));
      } else if (type.compare("SOCIAL") == 0) {
        std::string socialType = tokens[3];
        if (socialTaxers[year][jurisdiction].count(socialType) == 0) {
          socialTaxers[year][jurisdiction][socialType] = new TieredTaxer(
              currency);
        }
        socialTaxers[year][jurisdiction][socialType]->addTier(
            boost::lexical_cast<double>(tokens[4]) * currency,
            boost::lexical_cast<double>(tokens[5]));
      } else if (type.compare("EXEMPT") == 0) {
        exemptionAmtPerJuris[year][jurisdiction] = boost::lexical_cast<double>(
            tokens[3]) * currency;
      } else {
        fprintf(stderr, "Unknown type: %s\n", type.c_str());
      }
    }
  }

 public:
  ~TaxDictionary() {
    // TODO(amcp) delete all the tax tier objects
  }

  void addIncomeTaxer(int year, std::string jurisdiction,
                      TieredTaxer* taxer) {
    incomeTaxers[year][jurisdiction] = taxer;
  }

  void addSocialTaxer(int year, std::string jurisdiction,
                      std::string type, TieredTaxer* taxer) {
    socialTaxers[year][jurisdiction][type] = taxer;
  }

  void setExemptionAmount(int year, std::string jurisdiction,
                          QuantLib::Money amt) {
    exemptionAmtPerJuris[year][jurisdiction] = amt;
  }

  TieredTaxer* getIncomeTaxer(int year,
                              const std::string jurisdiction) {
    std::stringstream msg1;
    msg1 << "Income taxer data missing for year " << year << ".";
    QL_REQUIRE(incomeTaxers.count(year) > 0, msg1.str());
    std::map<int, IncomeTaxerMap>::const_iterator yearIt =
        incomeTaxers.find(year);
    IncomeTaxerMap map = (*yearIt).second;

    std::stringstream msg2;
    msg1 << "Income taxer data missing for jurisdiction " << jurisdiction
         << " in year " << year << ".";
    QL_REQUIRE(map.count(jurisdiction) > 0, msg2.str());
    IncomeTaxerMap::const_iterator it = map.find(jurisdiction);
    return (*it).second;
  }

  QuantLib::Money getExemptionAmount(int year,
                                     const std::string jurisdiction) {
    std::stringstream msg1;
    msg1 << "Exemption amount data missing for year " << year << ".";
    QL_REQUIRE(exemptionAmtPerJuris.count(year) > 0, msg1.str());
    std::map<int, ExemptionMap>::const_iterator yearIt =
        exemptionAmtPerJuris.find(year);
    ExemptionMap map = (*yearIt).second;

    std::stringstream msg2;
    msg1 << "Exemption amount data missing for jurisdiction " << jurisdiction
         << " in year " << year << ".";
    QL_REQUIRE(map.count(jurisdiction) > 0, msg2.str());
    ExemptionMap::const_iterator it = map.find(jurisdiction);
    return (*it).second;
  }

  IncomeTaxerMap getSocialTaxers(
      int year, const std::string jurisdiction) {
    std::stringstream msg1;
    msg1 << "Social taxer data missing for year " << year << ".";
    QL_REQUIRE(socialTaxers.count(year) > 0, msg1.str());
    std::map<int, SocialTaxerMap>::const_iterator yearIt =
        socialTaxers.find(year);
    SocialTaxerMap map = (*yearIt).second;

    std::stringstream msg2;
    msg1 << "Social taxer data missing for jurisdiction " << jurisdiction
         << " in year " << year << ".";
    QL_REQUIRE(map.count(jurisdiction) > 0, msg2.str());
    SocialTaxerMap::const_iterator it = map.find(jurisdiction);
    return (*it).second;
  }

  bool hasSocialTaxers(int year, const std::string jurisdiction) {
    if (socialTaxers.count(year) == 0) {
      return false;
    }

    std::map<int, SocialTaxerMap>::const_iterator yearIt =
        socialTaxers.find(year);
    SocialTaxerMap map = (*yearIt).second;
    return map.count(jurisdiction) > 0;
  }
};
}

#endif  // WORTH_TAX_TAXDICTIONARY_H_
