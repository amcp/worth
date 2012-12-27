/*
 * Utility.h
 *
 * Created on: 18 √ÎÙ 2012
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

#ifndef WORTH_UTILITY_H_
#define WORTH_UTILITY_H_

#include <ql/time/date.hpp>
#include <ql/time/schedule.hpp>

#include <ext/hash_map>
#include <vector>
#include <string>



namespace __gnu_cxx {
template<> struct hash<std::string> {
  size_t operator()(const std::string& x) const {
    return hash<const char*>()(x.c_str());
  }
};
}

namespace Worth {
class Utility {
 private:
  static Utility* theInstance;
  QuantLib::BusinessDayConvention convention;
  QuantLib::BusinessDayConvention terminationDateConvention;
  QuantLib::DateGeneration::Rule dateGenerationRule;

  Utility()
      : convention(QuantLib::ModifiedFollowing),
        terminationDateConvention(QuantLib::ModifiedFollowing),
        dateGenerationRule(QuantLib::DateGeneration::Forward) {
  }

 public:
  static Utility* getInstance() {
    if (theInstance == NULL) {
      theInstance = new Utility();
    }

    return theInstance;
  }

  QuantLib::Schedule generateSchedule(QuantLib::Period period,
                                      QuantLib::Date start,
                                      QuantLib::Date end,
                                      QuantLib::Calendar calendar) {
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
    return QuantLib::Schedule(start, end, period, calendar, convention,
                    terminationDateConvention, dateGenerationRule, false);
  }

  std::vector<std::string> readLines(const std::string& fname);
};
}

#endif  // WORTH_UTILITY_H_
