/*
 * JobPaymentEvent.h
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

#ifndef WORTH_JOBPAYMENTEVENT_H_
#define WORTH_JOBPAYMENTEVENT_H_

#include <ql/time/date.hpp>

#include <cstdio>
#include <cassert>
#include <string>

#include "worth/MyEvent.h"
#include "worth/Job.h"

namespace Worth {

class JobPaymentEvent : public MyEvent {
 private:
  Job* job;
  JobPayment* pmt;

 public:

  JobPaymentEvent(QuantLib::Date d, Job* j)
      : MyEvent(d),
        job(j),
        pmt(NULL) {
    assert(job != NULL);
  }

  //factory method
  static JobPaymentEvent* createJobPaymentEvent(QuantLib::Date d, Job* j) {
    return new JobPaymentEvent(d, j);
  }

  ~JobPaymentEvent() {
    delete pmt;
  }
  std::string toString() const {
    std::string result("NULL payment");
    if (pmt != NULL)
      result = pmt->toString();

    return result;
  }

  void apply(Sequencer* sequencer) {
    if (job->hasMorePayments()) {
      pmt = job->getNextPayment();
      DepositoryAccount* dd = this->job->getDDAccount();
      dd->creditAccount(pmt->getAmount());

      DepositoryAccount* employerRetire;
      employerRetire = job->getEmployerRetirementContributionAccount();
      employerRetire->creditAccount(pmt->getEmployerRetireContribution());

      DepositoryAccount* employeeRetire;
      employeeRetire = job->getEmployeeRetirementContributionAccount();
      employeeRetire->creditAccount(pmt->getEmployeeRetireContribution());

      sequencer->addEvent(new JobPaymentEvent(job->getCurrentPaymentDate(), job));

      printf("%s\n", pmt->toString().c_str());
      job->getUser()->addPayment(pmt);
    }
  }
};

}

#endif  // WORTH_JOBPAYMENTEVENT_H_
