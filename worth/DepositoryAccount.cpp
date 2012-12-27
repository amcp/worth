/*
 * DepositoryAccount.cpp
 *
 * Created on: Nov 1, 2011
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
