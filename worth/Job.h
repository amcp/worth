/**
 *  Copyright  2012 Alexander Patrikalakis
 */

#ifndef WORTH_JOB_H_
#define WORTH_JOB_H_

#include <ql/time/date.hpp>
#include <ql/time/schedule.hpp>
#include <ql/money.hpp>
#include <ql/currencies/america.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <ext/hash_map>

#include "worth/DepositoryAccount.h"
#include "worth/Utility.h"
#include "worth/tax/TieredTaxer.h"
#include "worth/MyEvent.h"
#include "worth/Payment.h"

class Job;

class JobPayment : public Payment {
 public:
  typedef __gnu_cxx ::hash_map<std::string, QuantLib::Money,
      __gnu_cxx ::hash<std::string> > StringMoneyMap;

 private:
  JobPayment()
      : Payment() {
  }
  QuantLib::Money grossEarnings;
  QuantLib::Money incomeWages;
  QuantLib::Money socialWages;
  StringMoneyMap incomeTaxes;
  __gnu_cxx ::hash_map<std::string, StringMoneyMap,
      __gnu_cxx ::hash<std::string> > socialTaxes;
  QuantLib::Money hourly;
  QuantLib::Money employeeRetireContribution;
  QuantLib::Money employerRetireContribution;

 public:
  JobPayment(
      QuantLib::Money amt,
      const QuantLib::Date date,
      QuantLib::Money gross,
      QuantLib::Money federal,
      QuantLib::Money SS,
      StringMoneyMap SIT,
      __gnu_cxx ::hash_map<std::string, StringMoneyMap,
          __gnu_cxx ::hash<std::string> > SOCT,
      QuantLib::Money hourlyRate, QuantLib::Money eeRetire,
      QuantLib::Money employerRetire)
      : Payment(date, amt),
        grossEarnings(gross),
        incomeWages(federal),
        socialWages(SS),
        incomeTaxes(SIT),
        socialTaxes(SOCT),
        hourly(hourlyRate),
        employeeRetireContribution(eeRetire),
        employerRetireContribution(employerRetire) {
  }
  ~JobPayment() {
  }

  std::string toString() const;
  friend std::ostream& operator<<(std::ostream& os, const JobPayment& pmt);

  inline QuantLib::Money getGrossEarnings() const {
    return grossEarnings;
  }
  inline QuantLib::Money getEmployeeRetireContribution() const {
    return employeeRetireContribution;
  }
  inline QuantLib::Money getEmployerRetireContribution() const {
    return employerRetireContribution;
  }
  inline const StringMoneyMap getIncomeTaxesPaid() const {
    return incomeTaxes;
  }
  inline const __gnu_cxx ::hash_map<std::string, StringMoneyMap,
      __gnu_cxx ::hash<std::string> > getSocialTaxesPaid() const {
    return socialTaxes;
  }
  inline QuantLib::Money getIncomeWages() const {
    return incomeWages;
  }
};

typedef __gnu_cxx ::hash_map<int,
    __gnu_cxx ::hash_map<int, std::vector<JobPayment*> > > PaymentMap;

class Person {
 private:
  QuantLib::Currency currency;
  std::vector<DepositoryAccount*> assets;
  std::vector<DepositoryAccount*> liabilities;
  PaymentMap paymentsPerCalendarYearAndMonth;
  __gnu_cxx ::hash_map<std::string, int, __gnu_cxx ::hash<std::string> > nominalExemptions;
  DepositoryAccount* mainDepository;
  // year->month->billName->vector
  __gnu_cxx ::hash_map<int,
      __gnu_cxx ::hash_map<int,
          __gnu_cxx ::hash_map<std::string, std::vector<Payment*>,
              __gnu_cxx ::hash<std::string> > > > billPayments;

 public:
  explicit Person(QuantLib::Currency cur)
      : currency(cur),
        mainDepository(NULL) {
  }

  Person(QuantLib::Currency cur, __gnu_cxx ::hash_map<std::string, int> ex);
  ~Person();

  inline void setMainDepository(DepositoryAccount* act) {
    mainDepository = act;
  }
  void addAsset(DepositoryAccount* asset);
  inline void addLiability(DepositoryAccount* liability) {
    liabilities.push_back(liability);
  }
  inline void addPayment(JobPayment* payment) {
    paymentsPerCalendarYearAndMonth[payment->getDate().year()][payment->getDate()
        .month()].push_back(payment);
  }
  inline void addBillPayment(std::string name, Payment* payment) {
    billPayments[payment->getDate().year()][payment->getDate().month()][name]
        .push_back(payment);
  }

  inline DepositoryAccount* getMainDepository() {
    return mainDepository;
  }
  inline QuantLib::Currency getCurrency() const {
    return currency;
  }
  QuantLib::Money getNetWorth();

  void generateYearEndSummary(int year) {
    __gnu_cxx ::hash_map<int,
        __gnu_cxx ::hash_map<std::string, std::vector<Payment*>,
            __gnu_cxx ::hash<std::string> > > billPaymentsInYear =
        billPayments[year];
    __gnu_cxx ::hash_map<int, std::vector<JobPayment*> > jobPaymentsInYear =
        paymentsPerCalendarYearAndMonth[year];
    __gnu_cxx ::hash_map<int,
        __gnu_cxx ::hash_map<std::string, std::vector<Payment*>,
            __gnu_cxx ::hash<std::string> > >::iterator monthIt;
    QuantLib::Money yearTotal = 0 * currency;

    QuantLib::Money cumulativeCashFlow = 0 * currency;
    for (monthIt = billPaymentsInYear.begin();
        monthIt != billPaymentsInYear.end(); monthIt++) {
      QuantLib::Money monthCashFlow = 0 * currency;
      QuantLib::Money monthTotalExpense = 0 * currency;
      std::cout << "Year: " << year << "; Month: " << (*monthIt).first
                << std::endl;
      __gnu_cxx ::hash_map<std::string, std::vector<Payment*>,
          __gnu_cxx ::hash<std::string> >::iterator catIt;
      for (catIt = (*monthIt).second.begin(); catIt != (*monthIt).second.end();
          catIt++) {
        QuantLib::Money categoryTotal = 0 * currency;
        std::vector<Payment*>::iterator pmtIt;
        for (pmtIt = (*catIt).second.begin(); pmtIt != (*catIt).second.end();
            pmtIt++) {
          categoryTotal += (*pmtIt)->getAmount();
        }
        std::cout << (*catIt).first << ": " << categoryTotal << "; ";
        monthTotalExpense += categoryTotal;
      }
      std::cout << std::endl;

      QuantLib::Money monthTotalIncome = 0 * currency;
      for (std::vector<JobPayment*>::iterator jobIt = jobPaymentsInYear[(*monthIt)
          .first].begin(); jobIt != jobPaymentsInYear[(*monthIt).first].end();
          jobIt++) {
        monthTotalIncome += (*jobIt)->getAmount();
      }

      monthCashFlow = monthTotalIncome - monthTotalExpense;
      cumulativeCashFlow += monthCashFlow;
      std::cout << "Year: " << year << "; Month: " << (*monthIt).first
                << "; Total expenses: " << monthTotalExpense
                << "; Total income: " << monthTotalIncome << "; Net cash flow: "
                << monthCashFlow << "; YTD cash flow: " << cumulativeCashFlow
                << std::endl;
      yearTotal += monthTotalExpense;
    }
  }
  __gnu_cxx ::hash_map<std::string, QuantLib::Money,
      __gnu_cxx ::hash<std::string> > generateTaxReturn(
      int year, QuantLib::Money deductionsInAllJurisdictions);
};

class Job {
 private:
  // base representation
  Person* user;
  QuantLib::Money balance;
  QuantLib::Money preIncomeTaxDeductionsPerPeriod;
  QuantLib::Money preSocialTaxDeductionsPerPeriod;
  QuantLib::Money taxableDeductionsPerPeriod;
  __gnu_cxx ::hash_map<std::string, QuantLib::Rate, __gnu_cxx ::hash<std::string> > taxJurisdictions;
  __gnu_cxx ::hash_map<std::string, int> exemptions;
  unsigned int effort;
  QuantLib::Period payPeriod;
  QuantLib::Date start;
  QuantLib::Date end;
  int accumulatedVacationHours;
  int vacationHoursTakenInPayPeriod;
  QuantLib::Money extraPayInPeriod;
  DepositoryAccount* ddAccount;
  DepositoryAccount* employerRetirementContributionAccount;
  DepositoryAccount* employeeRetirementContributionAccount;
  QuantLib::Rate employerRetirementGiftRatio;
  QuantLib::Rate employerRetirementMatchMaximum;
  QuantLib::Rate employerRetirementMatchRatio;
  // derived representation
  QuantLib::Schedule paySchedule;
  QuantLib::Schedule::const_iterator paymentIterator;

  Job()
      : effort(2080),
        accumulatedVacationHours(0),
        vacationHoursTakenInPayPeriod(0),
        employerRetirementGiftRatio(0),
        employerRetirementMatchMaximum(0),
        employerRetirementMatchRatio(0) {
    user = NULL;
    ddAccount = NULL;
    employerRetirementContributionAccount = NULL;
    employeeRetirementContributionAccount = NULL;
  }

 public:
  Job(Person* userIn,
      QuantLib::Money hourlyRate,
      double raiseRate,
      QuantLib::Calendar calendar,
      QuantLib::Money preIncomeTaxDeductions,
      QuantLib::Money preSocialTaxDeductions,
      QuantLib::Money taxableDeductions,
      __gnu_cxx ::hash_map<std::string, QuantLib::Rate,
          __gnu_cxx ::hash<std::string> > taxingJurisdictions,
      __gnu_cxx ::hash_map<std::string, int> ex, int hours,
      QuantLib::Period freq, QuantLib::Date startDate, QuantLib::Date endDate,
      int accumVacHrs, DepositoryAccount* dd,
      DepositoryAccount* employerContributionAccount,
      DepositoryAccount* employeeContributionAccount, double erReturementGift,
      double erRetirementMatchMaximum, double erRetirementMatchRatio);

  // copy constructor
  Job(const Job& rhs) {
    this->user = rhs.user;
    this->balance = rhs.balance;
    this->preIncomeTaxDeductionsPerPeriod = rhs.preIncomeTaxDeductionsPerPeriod;
    this->preSocialTaxDeductionsPerPeriod = rhs.preSocialTaxDeductionsPerPeriod;
    this->taxableDeductionsPerPeriod = rhs.taxableDeductionsPerPeriod;
    this->taxJurisdictions = rhs.taxJurisdictions;
    this->exemptions = rhs.exemptions;
    this->effort = rhs.effort;
    this->payPeriod = rhs.payPeriod;
    this->start = rhs.start;
    this->end = rhs.end;
    this->accumulatedVacationHours = rhs.accumulatedVacationHours;
    this->vacationHoursTakenInPayPeriod = rhs.vacationHoursTakenInPayPeriod;
    this->extraPayInPeriod = rhs.extraPayInPeriod;
    this->ddAccount = rhs.ddAccount;
    this->employerRetirementContributionAccount = rhs
        .employerRetirementContributionAccount;
    this->employeeRetirementContributionAccount = rhs
        .employeeRetirementContributionAccount;
    this->employerRetirementGiftRatio = rhs.employerRetirementGiftRatio;
    this->employerRetirementMatchMaximum = rhs.employerRetirementMatchMaximum;
    this->employerRetirementMatchRatio = rhs.employerRetirementMatchRatio;
    this->paySchedule = rhs.paySchedule;
    this->paymentIterator = this->paySchedule.begin();  // not the same! caution!
  }

  // assignment operator
  Job& operator=(const Job &rhs) {
    // Only do assignment if RHS is a different object from this. Self assignment check
    if (this != &rhs) {
      this->user = rhs.user;
      this->balance = rhs.balance;
      this->preIncomeTaxDeductionsPerPeriod = rhs
          .preIncomeTaxDeductionsPerPeriod;
      this->preSocialTaxDeductionsPerPeriod = rhs
          .preSocialTaxDeductionsPerPeriod;
      this->taxableDeductionsPerPeriod = rhs.taxableDeductionsPerPeriod;
      this->taxJurisdictions = rhs.taxJurisdictions;
      this->exemptions = rhs.exemptions;
      this->effort = rhs.effort;
      this->payPeriod = rhs.payPeriod;
      this->start = rhs.start;
      this->end = rhs.end;
      this->accumulatedVacationHours = rhs.accumulatedVacationHours;
      this->vacationHoursTakenInPayPeriod = rhs.vacationHoursTakenInPayPeriod;
      this->extraPayInPeriod = rhs.extraPayInPeriod;
      this->ddAccount = rhs.ddAccount;
      this->employerRetirementContributionAccount = rhs
          .employerRetirementContributionAccount;
      this->employeeRetirementContributionAccount = rhs
          .employeeRetirementContributionAccount;
      this->employerRetirementGiftRatio = rhs.employerRetirementGiftRatio;
      this->employerRetirementMatchMaximum = rhs.employerRetirementMatchMaximum;
      this->employerRetirementMatchRatio = rhs.employerRetirementMatchRatio;
      this->paySchedule = rhs.paySchedule;
      this->paymentIterator = this->paySchedule.begin();  // not the same! caution!
    }

    return *this;
  }

  Person* getUser() {
    return user;
  }

  ~Job();

  // queries
  inline unsigned int getScheduledHours() const {
    return effort;
  }
  inline QuantLib::Rate getFte() const {
    return static_cast<double>(effort) / static_cast<double>(2080);
  }
  inline QuantLib::Money getHourlyRate() const {
    return this->balance;
  }
  inline void setHourlyRate(QuantLib::Money stuff) {
    this->balance = stuff;
  }
  inline QuantLib::Money getBaseSalary() const {
    return getHourlyRate() * getScheduledHours();
  }
  inline QuantLib::Period getPayFrequency() const {
    return this->payPeriod;
  }
  inline QuantLib::Money getPreIncomeTaxDeductionsPerPeriod() const {
    return this->preIncomeTaxDeductionsPerPeriod;
  }
  inline QuantLib::Money getPreSocialTaxDeductionsPerPeriod() const {
    return this->preSocialTaxDeductionsPerPeriod;
  }
  inline QuantLib::Money getTaxableDeductionsPerPeriod() const {
    return this->taxableDeductionsPerPeriod;
  }
  inline QuantLib::Date getStartDate() const {
    return start;
  }
  inline QuantLib::Date getEndDate() const {
    return end;
  }
  inline int getAccumulatedVacationHours() {
    return accumulatedVacationHours;
  }
  inline int getVacationHoursTakenInPayPeriod() {
    return vacationHoursTakenInPayPeriod;
  }
  inline QuantLib::Money getExtraPayInPeriod() {
    return extraPayInPeriod;
  }
  inline void setAccumulatedVacationHours(int i) {
    accumulatedVacationHours = i;
  }
  inline void setVacationHoursTakenInPayPeriod(int i) {
    vacationHoursTakenInPayPeriod = i;
  }
  inline void setExtraPayInPeriod(QuantLib::Money m) {
    extraPayInPeriod = m;
  }
  inline QuantLib::Currency getCurrency() {
    return balance.currency();
  }
  inline QuantLib::Rate getEmployerRetirementGiftRate() const {
    return this->employerRetirementGiftRatio;
  }
  inline void setEmployerRetirementGiftRate(QuantLib::Rate in) {
    this->employerRetirementGiftRatio = in;
  }

  DepositoryAccount* getDDAccount() {
    return ddAccount;
  }
  DepositoryAccount* getEmployerRetirementContributionAccount() {
    return employerRetirementContributionAccount;
  }
  DepositoryAccount* getEmployeeRetirementContributionAccount() {
    return employeeRetirementContributionAccount;
  }

  QuantLib::Date getCurrentPaymentDate() const {
    return *paymentIterator;
  }

  inline unsigned int getPayPeriodsPerYear() const {
    unsigned int result = 1;
    if (payPeriod.units() == QuantLib::Weeks) {
      result = 52 / payPeriod.length();
    } else if (payPeriod.units() == QuantLib::Months) {
      result = 12 / payPeriod.length();
    }

    return result;
  }

  inline QuantLib::Money getAdjustedAnnualIncome(
      QuantLib::Money additionalPerPayPeriod) const {
    return getAdjustedAnnualSocialIncome(additionalPerPayPeriod)
        - preIncomeTaxDeductionsPerPeriod * (getPayPeriodsPerYear());
  }

  inline QuantLib::Money getAdjustedAnnualSocialIncome(
      QuantLib::Money additionalPerPayPeriod) const {
    return getBaseSalary()
        - preSocialTaxDeductionsPerPeriod * (getPayPeriodsPerYear())
        + additionalPerPayPeriod * getPayPeriodsPerYear();
  }

  // immutable factory methods
  inline void setTaxingJurisdictions(
      __gnu_cxx ::hash_map<std::string, QuantLib::Rate,
          __gnu_cxx ::hash<std::string> > juris) {
    this->taxJurisdictions = juris;
  }

  inline const __gnu_cxx ::hash_map<std::string, int> getExemptions() const {
    return exemptions;
  }

  inline void setExemptions(__gnu_cxx ::hash_map<std::string, int> ex) {
    exemptions = ex;
  }

  // mutable interface
  void setPreIncomeTaxDeductionsPerPeriod(QuantLib::Money deduct);
  void setPreSocialTaxDeductionsPerPeriod(QuantLib::Money deduct);
  void setTaxableDeductionsPerPeriod(QuantLib::Money deduct);

  JobPayment* getNextPayment();
  inline bool hasMorePayments() const {
    return paymentIterator != paySchedule.end();
  }
};

#endif  // WORTH_JOB_H_
