/*
 * Account.h
 *
 *  Created on: Nov 1, 2011
 *      Author: amcp
 */

#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include <iostream>
#include <ql/money.hpp>
#include <ql/time/calendar.hpp>

using namespace std;
using namespace QuantLib;

class Account {
 private:
  static unsigned int idSequence;
  unsigned int id;

  static void incrementIdSequence();

 protected:
  QuantLib::Money balance;
  QuantLib::Rate rate;
  QuantLib::Calendar exchangeCalendar;
  string name;

 public:
  Account(const Money& initialBalance, Rate initialRate,
          Calendar effectiveCalendar, string nameIn)
      : balance(initialBalance),
        rate(initialRate),
        exchangeCalendar(effectiveCalendar),
        name(nameIn) {
    this->id = Account::idSequence;
    Account::incrementIdSequence();
  }

  inline Money getBalance() {
    return balance;
  }
  inline Rate getRate() {
    return rate;
  }
  inline unsigned int getId() {
    return id;
  }
  inline Calendar getCalendar() {
    return exchangeCalendar;
  }
  inline Currency getCurrency() {
    return balance.currency();
  }

  inline void debitAccount(Money amt) {
    stringstream msg;
    msg << "Debits to account " << id << " must be made in "
        << balance.currency() << " and not " << amt.currency() << ".";
    QL_REQUIRE(amt.currency() == balance.currency(), msg.str());
    balance -= amt;
    if (balance < 0 * balance.currency()) {
      cerr << "Balance fell below zero to " << balance << endl;
    }
  }

  inline void creditAccount(Money amt) {
    stringstream msg;
    msg << "Credits to account " << id << " must be made in "
        << balance.currency() << " and not " << amt.currency() << ".";
    QL_REQUIRE(amt.currency() == balance.currency(), msg.str());
    balance += amt;
  }

  string toString() {
    stringstream msg;
    msg << "Name: " << name << "; Calendar: " << exchangeCalendar.name()
        << "; Rate: " << rate << "; Balance: " << balance;
    return msg.str();
  }
};

#endif /* ACCOUNT_H_ */
