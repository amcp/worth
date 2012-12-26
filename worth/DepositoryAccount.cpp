/*
 * DepositoryAccount.cpp
 *
 *  Created on: Nov 1, 2011
 *   Copyright  2012 Alexander Patrikalakis
 */

#include <ql/time/dategenerationrule.hpp>
#include <ql/time/schedule.hpp>
#include "worth/DepositoryAccount.h"

QuantLib::Schedule DepositoryAccount::generateCouponSchedule(
    const QuantLib::Date& start, const QuantLib::Date& end) {
  QuantLib::Period frequency(1, QuantLib::Months);
  QuantLib::Schedule sched(start, end, frequency, this->exchangeCalendar,
                           QuantLib::ModifiedFollowing,
                           QuantLib::ModifiedPreceding,
                           QuantLib::DateGeneration::Forward, false);

  return sched;
}
