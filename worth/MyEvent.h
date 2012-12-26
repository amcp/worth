/**
 *  Copyright 2012 Alexander Patrikalakis
 */

#ifndef WORTH_MYEVENT_H_
#define WORTH_MYEVENT_H_

#include <ql/time/date.hpp>

#include <queue>
#include <vector>
#include <string>
#include <iostream>
#include "worth/Utility.h"

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
        // cout << "Not executing event: " << first->toString() << endl;
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
    std::cout << "Dumping events in queue." << std::endl;
  }
};

#endif  // WORTH_MYEVENT_H_
