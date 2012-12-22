#ifndef MYEVENT_H_
#define MYEVENT_H_

#include <queue>
#include <vector>
#include <ql/time/date.hpp>
#include <iostream>
#include "Utility.h"

using namespace std;

class Sequencer;

class MyEvent {
 protected:
  MyEvent();
  QuantLib::Date exec;
 public:
  virtual ~MyEvent();
  MyEvent(QuantLib::Date d)
      : exec(d) {
  }
  QuantLib::Date getExecutionDate() {
    return exec;
  }
  virtual void apply(Sequencer& sequencer) = 0;
  virtual string toString() const = 0;

  struct comparator {
    bool operator ()(MyEvent* e1, MyEvent* e2) {
      return e1->exec > e2->exec;
    }
  };
};

typedef priority_queue<MyEvent*, vector<MyEvent*>, MyEvent::comparator> EventQueue;
typedef vector<MyEvent*> EventVector;

class Sequencer {
 private:
  QuantLib::Date begin;
  QuantLib::Date end;
  EventQueue events;
  EventVector unexecutedEvents;
  EventVector executedEvents;
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

    for (EventVector::iterator it = executedEvents.begin();
        it != executedEvents.end(); it++) {
      delete *it;
    }

    for (EventVector::iterator it = unexecutedEvents.begin();
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
        first->apply(*this);
        executedEvents.push_back(first);
      } else {
        //cout << "Not executing event: " << first->toString() << endl;
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
    cout << "Dumping events in queue." << endl;
  }
};

#endif /* MYEVENT_H_ */
