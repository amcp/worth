/*
 * TaxDictionary.h
 *
 *  Created on: 22 √ÎÙ 2012
 *      Author: amcp
 */

#ifndef TAXDICTIONARY_H_
#define TAXDICTIONARY_H_

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ext/hash_map>
#include <ql/money.hpp>
#include <boost/lexical_cast.hpp>
#include <ql/currencies/america.hpp>

#include "TieredTaxer.h"
#include "../Utility.h"

using namespace std;
using namespace __gnu_cxx;
using namespace QuantLib;

typedef hash_map<string, Money, hash<string > > ExemptionMap;
typedef hash_map<string, TieredTaxer* , hash<string > > IncomeTaxerMap;
typedef hash_map<string, hash_map<string, TieredTaxer*, hash<string > >, hash<string > > SocialTaxerMap;

class TaxDictionary {
private:
	static TaxDictionary* theInstance;
	TaxDictionary() {}
	hash_map<unsigned int, ExemptionMap> exemptionAmtPerJuris;
	hash_map<unsigned int, IncomeTaxerMap> incomeTaxers;
	hash_map<unsigned int, SocialTaxerMap> socialTaxers;
	Currency currency;

	TaxDictionary(string fname, Currency currencyIn) : currency(currencyIn){
		ifstream is;
		is.open(fname.c_str(), ios::in);

		if(is.is_open()) {
			string line;
			while(is.good()) {
				//get line
				getline(is,line);

				//get tokens in line
				stringstream strstr(line);
				istream_iterator<string> it(strstr);
				istream_iterator<string> end;
				vector<string> tokens;
				tokens.insert(tokens.begin(), it, end);

				if(tokens.size() == 0) {
					continue;
				}/* else {
					for(vector<string>::iterator tokit = tokens.begin(); tokit != tokens.end(); tokit++) {
						cout << *tokit << ", ";
					}
					cout << endl;
				}*/

				//process record
				unsigned int year = boost::lexical_cast<unsigned int>(tokens[0]);
				string type = tokens[1];
				string jurisdiction = tokens[2];
				if(type.compare("INCOME") == 0) {
					if(incomeTaxers[year].count(jurisdiction) == 0) {
						incomeTaxers[year][jurisdiction] = new TieredTaxer(currency);
					}
					incomeTaxers[year][jurisdiction]->addTier(boost::lexical_cast<double>(tokens[3]), boost::lexical_cast<double>(tokens[4]));
				} else if(type.compare("SOCIAL") == 0) {
					string socialType = tokens[3];
					if(socialTaxers[year][jurisdiction].count(socialType) == 0) {
						socialTaxers[year][jurisdiction][socialType] = new TieredTaxer(currency);
					}
					socialTaxers[year][jurisdiction][socialType]->addTier(boost::lexical_cast<double>(tokens[4]), boost::lexical_cast<double>(tokens[5]));
				} else if(type.compare("EXEMPT") == 0) {
					exemptionAmtPerJuris[year][jurisdiction] = boost::lexical_cast<double>(tokens[3]) * currency;
				} else {
					cerr << "Unknown type " << type << endl;
				}
		    }
		    is.close();
		}
	}

public:
	static TaxDictionary* getInstance() {
		if(theInstance == NULL) {
			theInstance = new TaxDictionary("/Users/amcp/Documents/workspace/worth/Debug/singleTaxTables.txt", USDCurrency());
		}
		return theInstance;
	}

	~TaxDictionary() {
		//TODO delete all the tax tier objects
	}

	void addIncomeTaxer(unsigned int year, string jurisdiction, TieredTaxer* taxer) {
		incomeTaxers[year][jurisdiction] = taxer;
	}

	void addSocialTaxer(unsigned int year, string jurisdiction, string type, TieredTaxer* taxer) {
		socialTaxers[year][jurisdiction][type] = taxer;
	}

	void setExemptionAmount(unsigned int year, string jurisdiction, Money amt) {
		exemptionAmtPerJuris[year][jurisdiction] = amt;
	}

	TieredTaxer* getIncomeTaxer(unsigned int year, const string jurisdiction) {
		stringstream msg1;
		msg1 << "Income taxer data missing for year " << year << ".";
		QL_REQUIRE(incomeTaxers.count(year) > 0, msg1.str());
		hash_map<unsigned int, IncomeTaxerMap>::const_iterator yearIt = incomeTaxers.find(year);
		IncomeTaxerMap map = (*yearIt).second;

		stringstream msg2;
		msg1 << "Income taxer data missing for jurisdiction " << jurisdiction << " in year " << year << ".";
		QL_REQUIRE(map.count(jurisdiction) > 0, msg2.str());
		IncomeTaxerMap::const_iterator it = map.find(jurisdiction);
		return (*it).second;
	}

	Money getExemptionAmount(unsigned int year, const string jurisdiction) {
		stringstream msg1;
		msg1 << "Exemption amount data missing for year " << year << ".";
		QL_REQUIRE(exemptionAmtPerJuris.count(year) > 0, msg1.str());
		hash_map<unsigned int, ExemptionMap>::const_iterator yearIt = exemptionAmtPerJuris.find(year);
		ExemptionMap map = (*yearIt).second;

		stringstream msg2;
		msg1 << "Exemption amount data missing for jurisdiction " << jurisdiction << " in year " << year << ".";
		QL_REQUIRE(map.count(jurisdiction) > 0, msg2.str());
		ExemptionMap::const_iterator it = map.find(jurisdiction);
		return (*it).second;
	}

	hash_map<string, TieredTaxer*, hash<string > > getSocialTaxers(unsigned int year, const string jurisdiction) {
		stringstream msg1;
		msg1 << "Social taxer data missing for year " << year << ".";
		QL_REQUIRE(socialTaxers.count(year) > 0, msg1.str());
		hash_map<unsigned int, SocialTaxerMap>::const_iterator yearIt = socialTaxers.find(year);
		SocialTaxerMap map = (*yearIt).second;

		stringstream msg2;
		msg1 << "Social taxer data missing for jurisdiction " << jurisdiction << " in year " << year << ".";
		QL_REQUIRE(map.count(jurisdiction) > 0, msg2.str());
		SocialTaxerMap::const_iterator it = map.find(jurisdiction);
		return (*it).second;
	}

	bool hasSocialTaxers(unsigned int year, const string jurisdiction) {
		if(socialTaxers.count(year) == 0) {
			return false;
		}

		hash_map<unsigned int, SocialTaxerMap>::const_iterator yearIt = socialTaxers.find(year);
		SocialTaxerMap map = (*yearIt).second;
		return map.count(jurisdiction) > 0;
	}
};

#endif /* TAXDICTIONARY_H_ */
