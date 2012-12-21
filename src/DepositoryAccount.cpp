/*
 * DepositoryAccount.cpp
 *
 *  Created on: Nov 1, 2011
 *      Author: amcp
 */

#include "ql/time/dategenerationrule.hpp"
#include "DepositoryAccount.h"

using namespace QuantLib;

Schedule DepositoryAccount::generateCouponSchedule(const Date& start,
                                                   const Date& end) {
  Period frequency(1, Months);
  Schedule sched(start, end, frequency, this->exchangeCalendar,
                 ModifiedFollowing, ModifiedPreceding, DateGeneration::Forward,
                 false);

  return sched;
}
