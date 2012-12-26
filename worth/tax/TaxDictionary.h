/*
 * TaxDictionary.h
 *
 *  Created on: 22 √ÎÙ 2012
 *   Copyright 2012 Alexander Patrikalakis
 */

#ifndef WORTH_TAX_TAXDICTIONARY_H_
#define WORTH_TAX_TAXDICTIONARY_H_

#include <boost/lexical_cast.hpp>
#include <ql/money.hpp>
#include <ql/currencies/america.hpp>

#include <ext/hash_map>
#include <string>
#include <sstream>
#include <vector>

#include "worth/tax/TieredTaxer.h"
#include "worth/Utility.h"

namespace Worth {
typedef __gnu_cxx ::hash_map<std::string, QuantLib::Money,
    __gnu_cxx ::hash<std::string> > ExemptionMap;
typedef __gnu_cxx ::hash_map<std::string, TieredTaxer*,
    __gnu_cxx ::hash<std::string> > IncomeTaxerMap;
typedef __gnu_cxx ::hash_map<std::string, IncomeTaxerMap,
    __gnu_cxx ::hash<std::string> > SocialTaxerMap;

class TaxDictionary {
 private:
  TaxDictionary() {
  }
  __gnu_cxx ::hash_map<unsigned int, ExemptionMap> exemptionAmtPerJuris;
  __gnu_cxx ::hash_map<unsigned int, IncomeTaxerMap> incomeTaxers;
  __gnu_cxx ::hash_map<unsigned int, SocialTaxerMap> socialTaxers;
  QuantLib::Currency currency;

  TaxDictionary(std::string fname, QuantLib::Currency currencyIn)
      : currency(currencyIn) {
    Utility* util = Utility::getInstance();

    std::vector<std::string> lines = util->readLines(fname);
    for (std::vector<std::string>::iterator lineIt = lines.begin();
        lineIt != lines.end(); ++lineIt) {
      // get tokens in line
      std::stringstream strstr(*lineIt);
      std::istream_iterator<std::string> it(strstr);
      std::istream_iterator<std::string> end;
      std::vector<std::string> tokens;
      tokens.insert(tokens.begin(), it, end);

      if (tokens.size() == 0) {
        continue;
      }
      // else {
      // std::vector<std::string>::iterator tokit
      // for (tokit = tokens.begin(); tokit != tokens.end(); tokit++) {
      // cout << *tokit << ", ";
      // }
      // cout << endl;
      // }

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
        std::cerr << "Unknown type " << type << std::endl;
      }
    }
  }

 public:
  ~TaxDictionary() {
    // TODO(amcp) delete all the tax tier objects
  }

  void addIncomeTaxer(unsigned int year, std::string jurisdiction,
                      TieredTaxer* taxer) {
    incomeTaxers[year][jurisdiction] = taxer;
  }

  void addSocialTaxer(unsigned int year, std::string jurisdiction,
                      std::string type, TieredTaxer* taxer) {
    socialTaxers[year][jurisdiction][type] = taxer;
  }

  void setExemptionAmount(unsigned int year, std::string jurisdiction,
                          QuantLib::Money amt) {
    exemptionAmtPerJuris[year][jurisdiction] = amt;
  }

  TieredTaxer* getIncomeTaxer(unsigned int year,
                              const std::string jurisdiction) {
    std::stringstream msg1;
    msg1 << "Income taxer data missing for year " << year << ".";
    QL_REQUIRE(incomeTaxers.count(year) > 0, msg1.str());
    __gnu_cxx ::hash_map<unsigned int, IncomeTaxerMap>::const_iterator yearIt =
        incomeTaxers.find(year);
    IncomeTaxerMap map = (*yearIt).second;

    std::stringstream msg2;
    msg1 << "Income taxer data missing for jurisdiction " << jurisdiction
         << " in year " << year << ".";
    QL_REQUIRE(map.count(jurisdiction) > 0, msg2.str());
    IncomeTaxerMap::const_iterator it = map.find(jurisdiction);
    return (*it).second;
  }

  QuantLib::Money getExemptionAmount(unsigned int year,
                                     const std::string jurisdiction) {
    std::stringstream msg1;
    msg1 << "Exemption amount data missing for year " << year << ".";
    QL_REQUIRE(exemptionAmtPerJuris.count(year) > 0, msg1.str());
    __gnu_cxx ::hash_map<unsigned int, ExemptionMap>::const_iterator yearIt =
        exemptionAmtPerJuris.find(year);
    ExemptionMap map = (*yearIt).second;

    std::stringstream msg2;
    msg1 << "Exemption amount data missing for jurisdiction " << jurisdiction
         << " in year " << year << ".";
    QL_REQUIRE(map.count(jurisdiction) > 0, msg2.str());
    ExemptionMap::const_iterator it = map.find(jurisdiction);
    return (*it).second;
  }

  __gnu_cxx::hash_map<std::string, TieredTaxer*, __gnu_cxx::hash<std::string> > getSocialTaxers(
      unsigned int year, const std::string jurisdiction) {
    std::stringstream msg1;
    msg1 << "Social taxer data missing for year " << year << ".";
    QL_REQUIRE(socialTaxers.count(year) > 0, msg1.str());
    __gnu_cxx ::hash_map<unsigned int, SocialTaxerMap>::const_iterator yearIt =
        socialTaxers.find(year);
    SocialTaxerMap map = (*yearIt).second;

    std::stringstream msg2;
    msg1 << "Social taxer data missing for jurisdiction " << jurisdiction
         << " in year " << year << ".";
    QL_REQUIRE(map.count(jurisdiction) > 0, msg2.str());
    SocialTaxerMap::const_iterator it = map.find(jurisdiction);
    return (*it).second;
  }

  bool hasSocialTaxers(unsigned int year, const std::string jurisdiction) {
    if (socialTaxers.count(year) == 0) {
      return false;
    }

    __gnu_cxx ::hash_map<unsigned int, SocialTaxerMap>::const_iterator yearIt =
        socialTaxers.find(year);
    SocialTaxerMap map = (*yearIt).second;
    return map.count(jurisdiction) > 0;
  }
};
}

#endif  // WORTH_TAX_TAXDICTIONARY_H_
