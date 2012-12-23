/*
 * Utility.h
 *
 *  Created on: 18 √ÎÙ 2012
 *      Author: amcp
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <vector>
#include <string>
#include <ext/hash_map>
#include <ql/time/date.hpp>
#include <ql/time/schedule.hpp>

using namespace std;
using namespace __gnu_cxx;
using namespace QuantLib;

namespace __gnu_cxx {
template<> struct hash<string> {
  size_t operator()(const string& x) const {
    return hash<const char*>()(x.c_str());
  }
};
}

namespace Worth {
class Utility {
 private:
  static Utility* theInstance;
  BusinessDayConvention convention;
  BusinessDayConvention terminationDateConvention;
  DateGeneration::Rule dateGenerationRule;

  Utility()
      : convention(ModifiedFollowing),
        terminationDateConvention(ModifiedFollowing),
        dateGenerationRule(DateGeneration::Forward) {
  }

 public:
  static Utility* getInstance() {
    if (theInstance == NULL) {
      theInstance = new Utility();
    }

    return theInstance;
  }

  Schedule generateSchedule(Period period, Date start, Date end,
                            Calendar calendar) {
    /*Date effectiveDate,
     const Date& terminationDate,
     const Period& tenor,
     const Calendar& calendar,
     BusinessDayConvention convention,
     BusinessDayConvention terminationDateConvention,
     DateGeneration::Rule rule,
     bool endOfMonth,
     const Date& firstDate = Date(),
     const Date& nextToLastDate = Date());*/
    return Schedule(start, end, period, calendar, convention,
                    terminationDateConvention, dateGenerationRule, false);
  }

  std::vector<std::string> readLines(const std::string& fname) ;
};
}

#endif /* UTILITY_H_ */
