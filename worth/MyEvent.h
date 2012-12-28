/**
 * MyEvent.h
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

#ifndef WORTH_MYEVENT_H_
#define WORTH_MYEVENT_H_

#include <ql/time/date.hpp>
#include <cstdio>
#include <queue>
#include <vector>
#include <string>
#include "worth/Utility.h"

namespace Worth {

class Sequencer;

class MyEvent {
 public:
  struct comparator;
  typedef std::vector<MyEvent*> EventVector;
  typedef std::priority_queue<MyEvent*, EventVector, comparator> EventQueue;
 protected:
  MyEvent();
  QuantLib::Date exec;
 public:
  virtual ~MyEvent();
  explicit MyEvent(QuantLib::Date d)
      : exec(d) {
  }
  QuantLib::Date getExecutionDate() {
    return exec;
  }
  virtual void apply(Sequencer* sequencer) = 0;
  virtual std::string toString() const = 0;

  struct comparator {
    bool operator()(MyEvent* e1, MyEvent* e2) {
      return e1->exec > e2->exec;
    }
  };
};

class Sequencer {
 private:
  QuantLib::Date begin;
  QuantLib::Date end;
  MyEvent::EventQueue events;
  MyEvent::EventVector unexecutedEvents;
  MyEvent::EventVector executedEvents;
  QuantLib::Date currentEventDate;

 public:
  Sequencer(QuantLib::Date beg, QuantLib::Date ending)
      : begin(beg),
        end(ending),
        currentEventDate(begin) {
  }
  ~Sequencer() {
    while (!events.empty()) {
      delete events.top();
      events.pop();
    }

    for (MyEvent::EventVector::iterator it = executedEvents.begin();
        it != executedEvents.end(); it++) {
      delete *it;
    }

    for (MyEvent::EventVector::iterator it = unexecutedEvents.begin();
        it != unexecutedEvents.end(); it++) {
      delete *it;
    }
  }

  void run() {
    if (events.empty())
      return;

    while (!events.empty()) {
      MyEvent* first = events.top();
      currentEventDate = first->getExecutionDate();
      events.pop();
      if (!(begin > currentEventDate || currentEventDate > end)) {
        first->apply(this);
        executedEvents.push_back(first);
      } else {
        printf("Not executing event: %s\n", first->toString().c_str());
        unexecutedEvents.push_back(first);
      }
    }
  }

  void addEvent(MyEvent* ev) {
    if (ev != NULL) {
      events.push(ev);
    }
  }

  void dumpEvents() {
    printf("Dumping events in queue.\n");
  }
};

}

#endif  // WORTH_MYEVENT_H_
