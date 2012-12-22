/*
 * JobPaymentEvent.cpp
 *
 *  Created on: 22 Ãëô 2012
 *      Author: amcp
 */

#include "JobPaymentEvent.h"

void JobPaymentEvent::apply(Sequencer& sequencer) {
  if (job.hasMorePayments()) {
    pmt = job.getNextPayment();
    this->job.getDDAccount()->creditAccount(pmt->getAmount());
    this->job.getEmployerRetirementContributionAccount()->creditAccount(
        pmt->getEmployerRetireContribution());
    this->job.getEmployeeRetirementContributionAccount()->creditAccount(
        pmt->getEmployeeRetireContribution());
    sequencer.addEvent(new JobPaymentEvent(job.getCurrentPaymentDate(), job));
    cout << *pmt << endl;
    job.getUser()->addPayment(pmt);
  }
}

