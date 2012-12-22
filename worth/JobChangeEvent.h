#ifndef JOBCHANGEEVENT_H_
#define JOBCHANGEEVENT_H_

#include <iostream>
#include <istream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include "MyEvent.h"
#include "Job.h"
#include "JobPaymentEvent.h"

using namespace std;
using namespace QuantLib;

class JobChangeEvent : public MyEvent {
 private:
  Job& job;
  string command;
  vector<string> tokens;
 public:
  JobChangeEvent(Job& j, string str, Date exec)
      : MyEvent(exec),
        job(j),
        command(str) {
    std::stringstream strstr(str);
    std::istream_iterator<std::string> it(strstr);
    std::istream_iterator<std::string> end;
    tokens.insert(tokens.begin(), it, end);
  }

  ~JobChangeEvent() {
    tokens.clear();
  }

  void apply(Sequencer& sequencer) {
    if (tokens.size() < 1) {
      return;
    }

    cout << exec << ": " << command << endl;

    double tempDouble = 0;
    unsigned int tempUint = 0;

    if (tokens[0].compare("HOURLY_RAISE") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch (const boost::bad_lexical_cast&) {
        cerr << "Unable to parse" << endl;
        tempDouble = 0;
      }

      Money temp(tempDouble, job.getCurrency());
      temp = temp + job.getHourlyRate();
      job.setHourlyRate(temp);
    } else if (tokens[0].compare("CHANGE_EMPLOYEE_403B_CONTRIBUTIONS") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch (const boost::bad_lexical_cast&) {
        cerr << "Unable to parse" << endl;
        tempDouble = 0;
      }
      job.setPreIncomeTaxDeductionsPerPeriod(tempDouble * job.getCurrency());
    } else if (tokens[0].compare("CHANGE_EXTRA_PAYMENT") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch (const boost::bad_lexical_cast&) {
        cerr << "Unable to parse" << endl;
        tempDouble = 0;
      }
      job.setExtraPayInPeriod(tempDouble * job.getCurrency());
    } else if (tokens[0].compare("CHANGE_EMPLOYER_RETIREMENT_GIFT") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch (const boost::bad_lexical_cast&) {
        cerr << "Unable to parse" << endl;
        tempDouble = 0;
      }
      job.setEmployerRetirementGiftRate(tempDouble);
    } else if (tokens[0].compare("CHANGE_PRESOCIAL_DEDUCTIONS") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch (const boost::bad_lexical_cast&) {
        cerr << "Unable to parse" << endl;
        tempDouble = 0;
      }
      job.setPreSocialTaxDeductionsPerPeriod(tempDouble * job.getCurrency());
    } else if (tokens[0].compare("CHANGE_PREINCOME_DEDUCTIONS") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch (const boost::bad_lexical_cast&) {
        cerr << "Unable to parse" << endl;
        tempDouble = 0;
      }
      job.setPreIncomeTaxDeductionsPerPeriod(tempDouble * job.getCurrency());
    } else if (tokens[0].compare("CHANGE_POSTTAX_DEDUCTIONS") == 0) {
      try {
        tempDouble = boost::lexical_cast<double>(tokens[1]);
      } catch (const boost::bad_lexical_cast&) {
        cerr << "Unable to parse" << endl;
        tempDouble = 0;
      }
      job.setTaxableDeductionsPerPeriod(tempDouble * job.getCurrency());
    } else if (tokens[0].compare("CHANGE_EXEMPTION") == 0) {
      hash_map<string, int, hash<string> > exemptions = job.getExemptions();
      try {
        tempUint = boost::lexical_cast<int>(tokens[2]);
      } catch (const boost::bad_lexical_cast&) {
        cerr << "Unable to parse" << endl;
        tempUint = 0;
      }
      exemptions[tokens[1]] = tempUint;
      job.setExemptions(exemptions);
    } else if (tokens[0].compare("START_JOB") == 0) {
      if (job.hasMorePayments())
        sequencer.addEvent(
            new JobPaymentEvent(job.getCurrentPaymentDate(), job));
    } else {
      cerr << "Unknown job event: " << tokens[0] << endl;
    }
  }

  string toString() const {
    return command;
  }
};

#endif /* JOBCHANGEEVENT_H_ */