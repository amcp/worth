/*
 * JobPaymentEvent.h
 *
 *  Created on: 22 Ãכפ 2012
 *      Author: amcp
 */

#ifndef JOBPAYMENTEVENT_H_
#define JOBPAYMENTEVENT_H_

#include <string>
#include <ql/time/date.hpp>
#include "MyEvent.h"
#include "Job.h"

class JobPaymentEvent : public MyEvent {
 private:
  Job& job;
  JobPayment* pmt;
 public:
  JobPaymentEvent(QuantLib::Date d, Job& j)
      : MyEvent(d),
        job(j),
        pmt(NULL) {
  }
  ~JobPaymentEvent() {
    delete pmt;
  }
  std::string toString() const {
    string result("NULL payment");
    if (pmt != NULL)
      result = pmt->toString();

    return result;
  }

  void apply(Sequencer& sequencer);
};

#endif /* JOBPAYMENTEVENT_H_ */
