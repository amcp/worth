/*
 * JobPaymentEvent.h
 *
 *  Created on: 22 Ãכפ 2012
 *  Copyright 2012 Alexander Patrikalakis
 */

#ifndef WORTH_JOBPAYMENTEVENT_H_
#define WORTH_JOBPAYMENTEVENT_H_

#include <ql/time/date.hpp>

#include <string>

#include "worth/MyEvent.h"
#include "worth/Job.h"

class JobPaymentEvent : public MyEvent {
 private:
  Job* job;
  JobPayment* pmt;
 public:
  JobPaymentEvent(QuantLib::Date d, Job* j)
      : MyEvent(d),
        job(j),
        pmt(NULL) {
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

  void apply(Sequencer* sequencer);
};

#endif  // WORTH_JOBPAYMENTEVENT_H_
