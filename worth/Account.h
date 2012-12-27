/*
 * Account.h
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

#ifndef WORTH_ACCOUNT_H_
#define WORTH_ACCOUNT_H_

#include <ql/money.hpp>
#include <ql/time/calendar.hpp>
#include <string>
#include <sstream>

class Account {
 private:
  static unsigned int idSequence;
  unsigned int id;

  static void incrementIdSequence();

 protected:
  QuantLib::Money balance;
  QuantLib::Rate rate;
  QuantLib::Calendar exchangeCalendar;
  std::string name;

 public:
  Account(const QuantLib::Money& initialBalance, QuantLib::Rate initialRate,
          QuantLib::Calendar effectiveCalendar, std::string nameIn)
      : balance(initialBalance),
        rate(initialRate),
        exchangeCalendar(effectiveCalendar),
        name(nameIn) {
    this->id = Account::idSequence;
    Account::incrementIdSequence();
  }

  inline QuantLib::Money getBalance() {
    return balance;
  }
  inline QuantLib::Rate getRate() {
    return rate;
  }
  inline unsigned int getId() {
    return id;
  }
  inline QuantLib::Calendar getCalendar() {
    return exchangeCalendar;
  }
  inline QuantLib::Currency getCurrency() {
    return balance.currency();
  }

  inline void debitAccount(QuantLib::Money amt) {
    std::stringstream msg;
    msg << "Debits to account " << id << " must be made in "
        << balance.currency() << " and not " << amt.currency() << ".";
    QL_REQUIRE(amt.currency() == balance.currency(), msg.str());
    balance -= amt;
    if (balance < 0 * balance.currency()) {
      //std::cerr << "Balance fell below zero to " << balance << std::endl;
    }
  }

  inline void creditAccount(QuantLib::Money amt) {
    std::stringstream msg;
    msg << "Credits to account " << id << " must be made in "
        << balance.currency() << " and not " << amt.currency() << ".";
    QL_REQUIRE(amt.currency() == balance.currency(), msg.str());
    balance += amt;
  }

  std::string toString() {
    std::stringstream msg;
    msg << "Name: " << name << "; Calendar: " << exchangeCalendar.name()
        << "; Rate: " << rate << "; Balance: " << balance;
    return msg.str();
  }
};

#endif  // WORTH_ACCOUNT_H_
