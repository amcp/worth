/**
 * JobChangeEvent.h
 *
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
#ifndef WORTH_JOBCHANGEEVENT_H_
#define WORTH_JOBCHANGEEVENT_H_

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdio>
#include <string>
#include <algorithm>
#include <vector>
#include "worth/MyEvent.h"
#include "worth/Job.h"
#include "worth/JobPaymentEvent.h"
#include "worth/Utility.h"

namespace Worth {

class JobChangeEvent : public MyEvent {
 private:
  Job* job;
  std::string command;
  std::vector<std::string> tokens;

 public:
  JobChangeEvent(Job* j, std::string str, QuantLib::Date exec)
      : MyEvent(exec),
        job(j),
        command(str) {
    boost::char_separator<char> sep(", ");
    boost::tokenizer<boost::char_separator<char> > tok(str, sep);
    std::copy(tok.begin, tok.end, tokens.begin());
  }

  ~JobChangeEvent() {
    tokens.clear();
  }

  void apply(Sequencer* sequencer) {
    Utility* util = Utility::getInstance();
    if (tokens.size() < 1) {
      return;
    }
    printf("%s: %s\n", util->convertDate(exec).c_str(), command);

    double tempDouble = 0;
    unsigned int tempUint = 0;

    if (tokens[0].compare("HOURLY_RAISE") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch(const boost::bad_lexical_cast&) {
        fprintf(stderr, "Unable to parse %s as a double.\n", tokens[1].c_str());
        tempDouble = 0;
      }

      QuantLib::Money temp(tempDouble, job->getCurrency());
      temp = temp + job->getHourlyRate();
      job->setHourlyRate(temp);
    } else if (tokens[0].compare("CHANGE_EMPLOYEE_403B_CONTRIBUTIONS") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch(const boost::bad_lexical_cast&) {
        fprintf(stderr, "Unable to parse %s as a double.\n", tokens[1].c_str());
        tempDouble = 0;
      }
      job->setPreIncomeTaxDeductionsPerPeriod(tempDouble * job->getCurrency());
    } else if (tokens[0].compare("CHANGE_EXTRA_PAYMENT") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch(const boost::bad_lexical_cast&) {
        fprintf(stderr, "Unable to parse %s as a double.\n", tokens[1].c_str());
        tempDouble = 0;
      }
      job->setExtraPayInPeriod(tempDouble * job->getCurrency());
    } else if (tokens[0].compare("CHANGE_EMPLOYER_RETIREMENT_GIFT") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch(const boost::bad_lexical_cast&) {
        fprintf(stderr, "Unable to parse %s as a double.\n", tokens[1].c_str());
        tempDouble = 0;
      }
      job->setEmployerRetirementGiftRate(tempDouble);
    } else if (tokens[0].compare("CHANGE_PRESOCIAL_DEDUCTIONS") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch(const boost::bad_lexical_cast&) {
        fprintf(stderr, "Unable to parse %s as a double.\n", tokens[1].c_str());
        tempDouble = 0;
      }
      job->setPreSocialTaxDeductionsPerPeriod(tempDouble * job->getCurrency());
    } else if (tokens[0].compare("CHANGE_PREINCOME_DEDUCTIONS") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch(const boost::bad_lexical_cast&) {
        fprintf(stderr, "Unable to parse %s as a double.\n", tokens[1].c_str());
        tempDouble = 0;
      }
      job->setPreIncomeTaxDeductionsPerPeriod(tempDouble * job->getCurrency());
    } else if (tokens[0].compare("CHANGE_POSTTAX_DEDUCTIONS") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch(const boost::bad_lexical_cast&) {
        fprintf(stderr, "Unable to parse %s as a double.\n", tokens[1].c_str());
        tempDouble = 0;
      }
      job->setTaxableDeductionsPerPeriod(tempDouble * job->getCurrency());
    } else if (tokens[0].compare("CHANGE_EXEMPTION") == 0) {
      NominalExemptionMap exemptions = job->getExemptions();
      try {
        tempUint = boost::lexical_cast<int>(tokens[2]);
      } catch(const boost::bad_lexical_cast&) {
        fprintf(stderr, "Unable to parse %s as a double.\n", tokens[2].c_str());
        tempUint = 0;
      }
      exemptions[tokens[1]] = tempUint;
      job->setExemptions(exemptions);
    } else if (tokens[0].compare("START_JOB") == 0) {
      if (job->hasMorePayments())
        MyEvent* ev = NULL;
        QuantLib::Date pmtDate = job->getCurrentPaymentDate();
        ev = JobPaymentEvent::createJobPaymentEvent(pmtDate, job);
        sequencer->addEvent(ev);
    } else {
      fprintf(stderr, "Unknown job event: %s\n", command.c_str());
    }
  }

  std::string toString() const {
    return command;
  }
};
}

#endif  // WORTH_JOBCHANGEEVENT_H_
