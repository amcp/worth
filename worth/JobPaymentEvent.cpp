/**
 * Copyright 2012 Alexander Patrikalakis
 */

#include "worth/JobPaymentEvent.h"

void JobPaymentEvent::apply(Sequencer* sequencer) {
  if (job->hasMorePayments()) {
    pmt = job->getNextPayment();
    this->job->getDDAccount()->creditAccount(pmt->getAmount());
    this->job->getEmployerRetirementContributionAccount()->creditAccount(
        pmt->getEmployerRetireContribution());
    this->job->getEmployeeRetirementContributionAccount()->creditAccount(
        pmt->getEmployeeRetireContribution());
    sequencer->addEvent(new JobPaymentEvent(job->getCurrentPaymentDate(), job));
    std::cout << *pmt << std::endl;
    job->getUser()->addPayment(pmt);
  }
}

